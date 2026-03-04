package model.image.core.matrix

import model.core.matrix.DenseMatrix
import model.core.matrix.errors.{BoundsError, MapError}
import org.scalatest.EitherValues.*
import org.scalatest.funspec.AnyFunSpec

class DenseMatrixSpec extends AnyFunSpec {
  describe("DenseMatrix") {
    val mat = DenseMatrix(
      Vector(
        Vector(1, 2, 3),
        Vector(4, 5, 6)
      ))

    it("should have correct dimensions") {
      assert(mat.rows == 2)
      assert(mat.cols == 3)
    }
    it("should return correct values") {
      assert(mat.get(0, 0).value == 1)
      assert(mat.get(1, 2).value == 6)
    }
    it("should return out-of-bounds error") {
      val getErr = mat.get(2, 0).left.value
      assert(getErr.isInstanceOf[BoundsError])
      assert(getErr.message.contains("(2, 0)"))
    }

    it("should update correctly") {
      val updated = mat.update(0, 1, 99).value
      assert(updated.get(0, 1).value == 99)
    }

    it("should check bounds in update method") {
      val updErr = mat.update(10, 10, 42).left.value
      assert(updErr.isInstanceOf[BoundsError])
    }

    it("should map correctly") {
      val mapped = mat.map(_ * 2).value
      assert(mapped.get(0, 0).value == 2)
      assert(mapped.get(1, 2).value == 12)
      val mapped_target = DenseMatrix(
        Vector(
          Vector(2, 4, 6),
          Vector(8, 10, 12)
        ))
      assert(mapped == mapped_target)
    }

    it("should map safely") {
      val mapErr = mat.map(_ => throw new RuntimeException("muhaha")).left.value
      assert(mapErr.isInstanceOf[MapError])
      assert(mapErr.message.startsWith("Mapping function threw an exception with message"))
    }
    it("should transpose correctly") {
      val transposed = mat.transpose
      assert(transposed.rows == 3)
      assert(transposed.cols == 2)
      assert(transposed.get(0, 0).value == 1)
      assert(transposed.get(2, 1).value == 6)

      assert(mat.T == transposed)

      assert(mat.transpose.transpose == mat)
    }

    it("should flipX correctly") {
      val flippedX = mat.flipX
      assert(flippedX.get(0, 0).value == 3)
      assert(flippedX.get(0, 2).value == 1)
      assert(mat.flipX.flipX == mat)
    }

    it("should flipY correctly") {
      val flippedY = mat.flipY
      assert(flippedY.get(0, 0).value == 4)
      assert(flippedY.get(1, 2).value == 3)
      assert(mat.flipY.flipY == mat)
    }

    it("should be immutable") {
      var updated = mat.update(0,0,42).value
      updated = updated.T
      updated = updated.flipX
      updated = updated.flipY
      updated = updated.map(_ * 2).value
      assert(mat != updated)
    }
  }



  describe("Empty DenseMatrix") {
    val emptyMat = DenseMatrix(Vector.empty[Vector[Int]])
    it("should have 0 rows and 0 cols") {
      assert(emptyMat.rows == 0)
      assert(emptyMat.cols == 0)
    }
    it("should fail on any get/update operation") {
      val getErr = emptyMat.get(0, 0).left.value
      assert(getErr.isInstanceOf[BoundsError])

      val updErr = emptyMat.update(0, 0, 42).left.value
      assert(updErr.isInstanceOf[BoundsError])
    }

    describe("map") {
      it("should be a no-op on empty matrix") {
        val mapped = emptyMat.map(_ + 1).value
        assert(mapped.rows == 0)
        assert(mapped.cols == 0)
      }
    }

    it("should transpose correctly") {
      val transposed = emptyMat.transpose
      assert(transposed.rows == 0 && transposed.cols == 0)
    }
    it("should flip correctly") {
      val flippedX = emptyMat.flipX
      val flippedY = emptyMat.flipY
      assert(flippedX.rows == 0 && flippedX.cols == 0)
      assert(flippedY.rows == 0 && flippedY.cols == 0)
    }
    describe("DenseMatrix companion object") {
      it("should construct new DenseMatrix from flat sequence") {
        val flat = Seq(0, 1, 2, 3)
        val res = DenseMatrix.fromFlatSeq(flat, 2, 2)
        assert(res.isDefined)
        val ref = DenseMatrix(Vector(Vector(0, 1), Vector(2, 3)))
        assert(res.get == ref, "DenseMatrix constructed from flat sequence not same as reference matrix")
      }
    }
  }
}
