import math
import socket
from enum import Enum
from threading import Thread
import numpy as np
import sys

DELIMITER = '\a\b'
FORMAT = 'ascii'


class Direction(Enum):
    RIGHT = 0
    FRONT = 1
    LEFT = 2
    BACK = 3

    @staticmethod
    def from_vector(vec: np.array):
        if np.array_equal(vec, np.array([1, 0])):
            return Direction.RIGHT
        elif np.array_equal(vec, np.array([0, 1])):
            return Direction.FRONT
        elif np.array_equal(vec, np.array([-1, 0])):
            return Direction.LEFT
        elif np.array_equal(vec, np.array([0, -1])):
            return Direction.BACK
        else:
            raise ValueError(f'Invalid vector: {vec}')

    def turn_left(self):
        return Direction((self.value + 1) % 4)

    def turn_right(self):
        return Direction((self.value - 1) % 4)


class bcolors:
    """
    source: https://stackoverflow.com/questions/287871/how-do-i-print-colored-text-to-the-terminal
    """
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def print_color(text, color):
    print(f'{color}{text}{bcolors.ENDC}')


class LoginFailed(Exception):
    pass


class LogicError(Exception):
    pass


class ServerThread(Thread):
    """
    Handles communication with a single client
    """

    def __init__(self, conn, address):
        # call Thread constructor
        super().__init__()
        # init fields
        self.destination = np.array([0, 0])
        self.socket = conn
        self.address = address
        self.msg = ''
        self.next_msg = ''
        self.stuck = False
        self.location = None
        self.heading = None
        self.recharging = False

    def send(self, msg):
        self.socket.sendall((str(msg) + '\a\b').encode('ascii'))

    def rcv_chunk(self, chunk_size):
        msg = self.socket.recv(chunk_size)
        if not msg:
            raise ConnectionError(f'Client [{self.address}] disconnected while server was receiving message')
        return msg.decode('ascii')

    def rcv(self, max_length=None):
        """
        Receives message from client
        :param max_length: maximum length of message, with delimiter
        :return: Full message without delimiter
        :raises SyntaxError: if message len(message) > max_length
        :raises LogicError: if client is recharging and sends a message
        """
        assert (max_length is None or max_length > 0)
        chunk_size = 2048
        counter = 0
        if max_length is not None:
            chunk_size = 1
        # reset buffers
        self.msg = self.next_msg
        self.next_msg = ''

        # receive message
        while DELIMITER not in self.msg:
            chunk = self.rcv_chunk(chunk_size)
            counter += len(chunk)
            self.msg += chunk
            if max_length is not None and counter >= max_length and chunk[-1] != '\b':
                raise SyntaxError(f'Client [{self.address}]: Message length {counter} > {max_length} (max_length)')

        # fix message collision
        del_pos = self.msg.find(DELIMITER)
        self.next_msg = self.msg[del_pos + 2:]
        self.msg = self.msg[:del_pos]
        # Charging
        if self.msg == Client.CLIENT_RECHARGING:
            self.recharge()
            # handle message that was expected
            return self.rcv(max_length=max_length)
        if self.msg == Client.CLIENT_FULL_POWER and not self.recharging:
            raise LogicError(f'Client [{self.address}] sent {Client.CLIENT_FULL_POWER} while not recharging')

        return self.msg

    def run(self):
        self.socket.settimeout(Server.TIMEOUT)
        try:
            self.authenticate()
            self.get_pos_head()
            self.navigate()
            self.pick_up()
            self.logout()
        except TimeoutError as e:
            print_color(e, bcolors.WARNING)
        except ConnectionError as e:
            print_color(e, bcolors.WARNING)
        except SyntaxError as e:
            print_color(e, bcolors.WARNING)
            self.send(Server.SERVER_SYNTAX_ERROR)
        except IndexError as e:
            print_color(e, bcolors.WARNING)
            self.send(Server.SERVER_KEY_OUT_OF_RANGE_ERROR)
        except LoginFailed as e:
            print_color(e, bcolors.WARNING)
            self.send(Server.SERVER_LOGIN_FAILED)
        except LogicError as e:
            print_color(e, bcolors.WARNING)
            self.send(Server.SERVER_LOGIC_ERROR)
        except socket.timeout:
            print_color(f'Client [{self.address}] timed out', bcolors.WARNING)
        finally:
            self.close()

    def turn_left(self):
        assert self.heading is not None
        self.send(Server.SERVER_TURN_LEFT)
        self.get_location()
        self.heading = self.heading.turn_left()

    def turn_right(self):
        assert self.heading is not None
        self.send(Server.SERVER_TURN_RIGHT)
        self.get_location()
        self.heading = self.heading.turn_right()

    def move_forward(self):
        """
        Moves robot forward
        :return: None
        """
        old_location = self.location
        self.send(Server.SERVER_MOVE)
        self.get_location()
        self.stuck = np.array_equal(self.location, old_location)

    def recharge(self):
        """
        Recharges robot
        :note sets recharging
        """
        self.recharging = True
        self.socket.settimeout(Server.TIMEOUT_RECHARGING)

        print_color(f'Client [{self.address}] is recharging', bcolors.OKCYAN)

        if self.rcv() != Client.CLIENT_FULL_POWER:
            raise LogicError(f'Expected {Client.CLIENT_FULL_POWER}, got {self.msg}')

        self.socket.settimeout(Server.TIMEOUT)
        self.recharging = False

    def pick_up(self) -> str:
        """
        Picks up secret message at [0, 0]
        :return: message
        """
        assert np.array_equal(self.location, self.destination)
        self.send(Server.SERVER_PICK_UP)
        msg = self.rcv(max_length=Client.MESSAGE_MAX_LENGTH)
        if len(msg) > 98:
            raise SyntaxError(f'Message length {len(msg)} > 98')
        print_color(f'Client [{self.address}] picked up message: {msg}', bcolors.OKGREEN)
        return msg

    def minimize_coord(self, coord_is_x: bool):
        """
        Minimizes coordinate
        :param coord_is_x: True for x, False for y
        :return: None
        """
        desired_heading = None

        # get vector from current location to destination and translate it into direction
        if coord_is_x:
            desired_heading = np.array([-math.copysign(1, self.location[0]), 0])
        else:
            desired_heading = np.array([0, -math.copysign(1, self.location[1])])
        self.adjust_heading(Direction.from_vector(desired_heading))

        coord_idx = 0 if coord_is_x else 1

        # move forward until obstacle is hit
        while not self.stuck and self.location[coord_idx] != 0:
            self.move_forward()

        # special case: stuck, facing the destination with the other coord equal to 0
        # in this case the obstacle can be dodged
        if self.stuck and self.location[1 - coord_idx] == 0 and self.location[coord_idx] != 0:
            self.dodge()

    def adjust_heading(self, new_heading: Direction):
        """
        Adjusts heading of client
        :param new_heading: new heading
        :return: None
        """
        assert self.heading is not None
        if self.heading == new_heading:
            return
        elif new_heading == self.heading.turn_left():
            self.turn_left()
        elif new_heading == self.heading.turn_right():
            self.turn_right()
        else:
            self.turn_left()
            self.turn_left()

    def get_pos_head(self):
        """
        Gets initial position and heading of client
        """
        self.send(Server.SERVER_TURN_RIGHT)
        self.get_location()
        # already at destination
        if np.array_equal(self.location, np.array([0, 0])):
            return

        prev_loc = self.location
        self.move_forward()  # move_forward changes self.location
        # while robot is stuck, turn right and try to move forward
        while np.array_equal(prev_loc, self.location):
            prev_loc = self.location

            self.send(Server.SERVER_TURN_RIGHT)
            self.get_location()

            self.move_forward()

        self.heading = Direction.from_vector(self.location - prev_loc)

    def close(self):
        print_color(f'Closing connection [{self.address}]', bcolors.FAIL)
        self.socket.close()

    @staticmethod
    def hash(name: str):
        assert (len(name) <= 16)
        tmp = 0
        for letter in name:
            tmp += ord(letter)
        return (tmp * 1000) % 65536

    def navigate(self):
        """
        Navigates robot to [0, 0]
        """
        print_color(f'Navigating [{self.address}] from {tuple(self.location)} to [0, 0]', bcolors.OKBLUE)
        minimize_x = True
        prev_loc = self.location
        while not np.array_equal(self.location, self.destination):
            self.minimize_coord(minimize_x)
            self.stuck = False
            minimize_x = not minimize_x

    def dodge(self):
        """
        Dodges an obstacle around its left edge.
        Obstacle is assumed to be in front of robot
        All 8 fields around obstacle are free
        :return: None
        """
        self.turn_left()
        self.move_forward()
        self.turn_right()
        self.move_forward()
        self.move_forward()
        self.turn_right()
        self.move_forward()
        self.turn_left()

    def authenticate(self):
        # Get username
        self.rcv(max_length=Client.USERNAME_MAX_LENGTH)
        name = self.msg

        # KEY REQUEST
        self.send(Server.SERVER_KEY_REQUEST)
        client_key = self.rcv(max_length=12)
        try:
            client_key = int(client_key)
        except ValueError:
            raise SyntaxError(f'Key [{client_key}] is not an integer')
        if client_key < 0 or client_key > 4:
            raise IndexError(f'Key [{client_key}] out of range [0, 4]')

        # SERVER_CONFIRMATION
        server_conf = (self.hash(name)
                       + Server.KEY_PAIRS[client_key][0]) % 65536
        self.send(server_conf)

        client_conf = self.rcv(Client.CONFIRMATION_MAX_LENGTH)

        # Verify client confirmation
        if client_conf.strip() != client_conf:
            raise SyntaxError(f'Client confirmation [{client_conf}] contains whitespace')
        try:
            client_conf = int(client_conf)
        except ValueError:
            raise SyntaxError(f'Client confirmation [{client_conf}] is not an integer')

        if client_conf != (self.hash(name) + Server.KEY_PAIRS[client_key][1]) % 65536:
            raise LoginFailed("Client confirmation doesn't match")

        # LOGIN SUCCESSFUL
        self.send(Server.SERVER_OK)
        print_color(f'Client [{name}] authenticated successfully.', bcolors.OKGREEN)

    def get_location(self):
        msg = self.rcv(max_length=Client.OK_MAX_LENGTH)
        if msg.strip() != msg:
            raise SyntaxError(f'Location message [{msg}] encased by whitespace')
        msg = msg.split()
        if len(msg) != 3 or msg[0] != 'OK':
            raise SyntaxError(f'Invalid location message: {msg}')  # ok, x, y
        try:
            location = tuple(int(x) for x in msg[1:])
        except ValueError:
            raise SyntaxError(f'Invalid location message: {msg}')
        location = np.array(location)

        self.location = location

    def logout(self):
        self.send(Server.SERVER_LOGOUT)
        print_color(f'Client [{self.address}] logged out', bcolors.OKBLUE)


class Server:
    """
    Accepts new clients and creates a new thread for each client.
    heavily inspired by https://docs.python.org/3/howto/sockets.html#socket-howto
    """
    # server messages
    SERVER_MOVE = '102 MOVE'
    SERVER_TURN_LEFT = '103 TURN LEFT'
    SERVER_TURN_RIGHT = '104 TURN RIGHT'
    SERVER_PICK_UP = '105 GET MESSAGE'
    SERVER_LOGOUT = '106 LOGOUT'
    SERVER_KEY_REQUEST = '107 KEY REQUEST'
    SERVER_OK = '200 OK'
    SERVER_LOGIN_FAILED = '300 LOGIN FAILED'
    SERVER_SYNTAX_ERROR = '301 SYNTAX ERROR'
    SERVER_LOGIC_ERROR = '302 LOGIC ERROR'
    SERVER_KEY_OUT_OF_RANGE_ERROR = '303 KEY OUT OF RANGE'
    # timeouts
    TIMEOUT = 1
    TIMEOUT_RECHARGING = 5
    KEY_PAIRS = (
        #  (server, client), idx being key ID
        (23019, 32037),
        (32037, 29295),
        (18789, 13603),
        (16443, 29533),
        (18189, 21952)
    )

    def __init__(self, port=3999, addr='localhost', max_queue=5):
        # socket should be TCP/IP (INET/STREAM)
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.threads = []
        self.clients = []
        self.port = port
        self.addr = addr
        self.max_queue = max_queue

    def listen(self):
        self.server_socket.bind((self.addr, self.port))
        self.server_socket.listen(self.max_queue)
        print(f'listening on [{self.addr}, {self.port}]')

    def start(self):
        print('Server starting')
        # the server will listen for new connections
        self.listen()
        while True:
            # accept new client
            client = (client_socket, client_address) = self.server_socket.accept()
            print_color(f'Accepted new client [{client_address}]', bcolors.OKBLUE)
            self.handle_client(*client)

    def send(self, msg):
        for thread in self.threads:
            thread.send(msg.encode('ascii'))

    def close(self):
        self.server_socket.close()

    # runs connections in new threads
    def handle_client(self, client_socket, client_address):
        self.clients.append(client_socket)
        # new thread for client
        thread = ServerThread(client_socket, client_address)
        self.threads.append(thread)
        thread.start()


class Client:
    CLIENT_RECHARGING = 'RECHARGING'
    CLIENT_FULL_POWER = 'FULL POWER'
    # all max length are with delimiter (\a\b)
    USERNAME_MAX_LENGTH = 20
    KEY_ID_MAX_LENGTH = 5
    CONFIRMATION_MAX_LENGTH = 7
    OK_MAX_LENGTH = 12
    MESSAGE_MAX_LENGTH = 100


if __name__ == "__main__":
    server = None
    try:
        server = Server()
        server.start()
    except KeyboardInterrupt:
        print_color('Server shutting down', bcolors.WARNING)
        if server:
            server.close()
        sys.exit(0)
