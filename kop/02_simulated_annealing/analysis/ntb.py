import marimo

__generated_with = "0.19.2"
app = marimo.App(width="medium")


@app.cell
def _():
    import marimo as mo
    import pandas as pd
    import numpy as np
    import pathlib
    from collections import defaultdict

    import matplotlib.pyplot as plt

    plt.style.use("ggplot")
    plt.rcParams["figure.facecolor"] = "white"
    plt.rcParams["axes.facecolor"] = "white"

    plt.rcParams["text.color"] = "black"
    plt.rcParams["axes.labelcolor"] = "black"
    plt.rcParams["xtick.color"] = "black"
    plt.rcParams["ytick.color"] = "black"


    NTB_PATH = pathlib.Path(__file__)
    DATA_PATH = NTB_PATH.parent.parent / "data"
    RESULTS_PATH = NTB_PATH.parent.parent / "results"
    REPORT_PATH = NTB_PATH.parent.parent / "report"
    import seaborn as sns
    return DATA_PATH, REPORT_PATH, RESULTS_PATH, defaultdict, pd, plt, sns


@app.cell
def _(DATA_PATH, defaultdict, pd):
    def read_reference():
        reference_solutions = defaultdict(list)
        for opt in DATA_PATH.rglob("*-opt.dat"):
            set_name = opt.stem.replace("-opt", "")

            with open(opt, "r") as f:
                lines = f.readlines()
            for line in lines:
                split = line.split()
                instance_id = split[0].split("-")[-1]
                reference_solutions["set_name"] += [set_name]
                reference_solutions["instance_id"] += [instance_id]
                reference_solutions["weight"] += [split[1]]
                assig = " ".join(split[2:])
                reference_solutions["assignment"] += [assig]
        return pd.DataFrame(reference_solutions).set_index(
            ["set_name", "instance_id"]
        )
    return (read_reference,)


@app.cell
def _(read_reference):
    reference = read_reference()
    reference
    return (reference,)


@app.cell
def _(RESULTS_PATH, defaultdict, pd):
    def read_results():
        results = defaultdict(list)
        for results_file in RESULTS_PATH.rglob("*.mwcnf"):
            set_name = results_file.parent.name
            instance_id = results_file.stem.split("-")[-1]

            with open(results_file, "r") as txt:
                runs = txt.readlines()
            for run in runs:
                run = run.strip()
                results["set_name"] += [set_name]
                results["instance_id"] += [instance_id]
                split = run.split()
                weight = split[0]
                assg = " ".join(split[1:])
                results["assignment"] += [assg.strip()]
                results["weight"] += [weight.strip()]
        return pd.DataFrame(results)
    return (read_results,)


@app.cell
def _(read_results):
    results = read_results()
    results
    return (results,)


@app.cell
def _(results):
    results_unique = (
        results.groupby(["set_name", "instance_id", "weight", "assignment"])
        .size()
        .reset_index(name="count", drop=False)
    )
    assert not results_unique.duplicated(
        ["set_name", "instance_id", "weight", "assignment"]
    ).any(), "Found exact duplicate rows"
    results_unique
    return (results_unique,)


@app.cell
def _(reference, results_unique):
    comparison = results_unique.merge(
        reference,
        left_on=["set_name", "instance_id"],
        right_index=True,
        how="left",
        suffixes=("_result", "_reference"),
    )
    comparison
    return (comparison,)


@app.cell
def _(comparison):
    comparison["solved"] = comparison["assignment_result"].str.strip() != "0"
    comparison["optimal"] = (
        comparison["weight_result"] == comparison["weight_reference"]
    )
    comparison["weight_diff"] = comparison["weight_result"].astype(
        int
    ) - comparison["weight_reference"].astype(int)
    return


@app.cell
def _(comparison):
    comparison
    return


@app.cell
def _(comparison, pd):
    agg = (
        comparison.groupby("set_name")
        .apply(
            lambda g: pd.Series(
                {
                    "total_runs": g["count"].sum(),
                    "solved_runs": (g["solved"] * g["count"]).sum(),
                    "optimal_runs": (g["optimal"] * g["count"]).sum(),
                }
            ),
            include_groups=False,
        )
        .reset_index()
    )

    agg["solved_pct"] = (agg["solved_runs"] / agg["total_runs"] * 100).round(2)
    agg["optimal_pct"] = (agg["optimal_runs"] / agg["total_runs"] * 100).round(2)
    agg.sort_values("set_name", inplace=True)
    agg[["set_name", "solved_pct", "optimal_pct"]]
    return


@app.cell
def _(RESULTS_PATH, pd):
    def read_convergence():
        dfs = []
        print(RESULTS_PATH)
        for csv_file in RESULTS_PATH.rglob("*.csv"):
            instance_name = csv_file.stem.rsplit("-run_", 1)[0]
            run_id = int(csv_file.stem.rsplit("-run_", 1)[1])
            set_name = csv_file.parent.name

            df = pd.read_csv(csv_file)
            df["set_name"] = set_name
            df["instance"] = instance_name
            df["run_id"] = run_id
            df["step"] = range(len(df))
            dfs.append(df)

        return dfs
    return (read_convergence,)


@app.cell
def _(read_convergence):
    convergence = read_convergence()
    convergence
    return (convergence,)


@app.cell
def _(plt, reference):
    def plot_convergence(sample):
        set_name = sample["set_name"].iloc[0]
        instance_id = sample["instance"].iloc[0].split("-")[-1]
        optimal = int(reference.loc[(set_name, instance_id), "weight"])

        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
        ax1.plot(sample["step"], sample["objective"], color="red")
        ax1.axhline(
            optimal, color="blue", linestyle="--", label=f"Optimal ({optimal})"
        )
        ax1.set_xlabel("Step")
        ax1.set_ylabel("Objective")
        ax1.set_title(f"Objective - {set_name}")
        ax1.legend()

        ax2.plot(sample["step"], sample["unsat"], color="blue")
        ax2.axhline(0, color="blue", linestyle="--", label="Feasible (0)")
        ax2.set_xlabel("Step")
        ax2.set_ylabel("Unsatisfied Clauses")
        ax2.set_title(f"Unsat - {set_name}")
        ax2.legend()

        fig.tight_layout()
        return fig
    return (plot_convergence,)


@app.cell
def _(REPORT_PATH, convergence, plot_convergence):
    sample = convergence[8]
    fig_single = plot_convergence(sample)
    fig_single.savefig(REPORT_PATH / "convergence_single.pdf", bbox_inches="tight")
    fig_single
    return


@app.cell
def _(convergence, pd):
    conv_df = pd.concat(convergence, ignore_index=True)
    return (conv_df,)


@app.cell
def _(plt, reference, sns):
    def plot_convergence_runs(df, instance, errorbar=None):
        """Plot objective and unsat convergence for an instance with moving average."""
        window_pct = 0.1
        steps = 5000  # zoomed steps
        hue = "run_id"

        subset = df[df["instance"] == instance].copy()
        set_name = subset["set_name"].iloc[0]
        instance_id = instance.split("-")[-1]
        optimal = int(reference.loc[(set_name, instance_id), "weight"])
        subset = subset.sort_values([hue, "step"])
        window = max(1, int(subset["step"].max() * window_pct))
        subset[hue] = subset[hue].astype(str)
        subset["objective_ma"] = subset.groupby(hue)["objective"].transform(
            lambda x: x.rolling(window=window, min_periods=1).mean()
        )
        subset["unsat_ma"] = subset.groupby(hue)["unsat"].transform(
            lambda x: x.rolling(window=window, min_periods=1).mean()
        )
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        fig.suptitle(f"{set_name}/{instance_id}")

        # full
        sns.lineplot(
            data=subset,
            x="step",
            y="objective_ma",
            hue=hue,
            palette="husl",
            alpha=0.7,
            errorbar=errorbar,
            ax=axes[0, 0],
            legend=False,
        )
        axes[0, 0].axhline(
            optimal, color="blue", linestyle="--", label=f"Optimal ({optimal})"
        )
        axes[0, 0].set_xlabel("Step")
        axes[0, 0].set_ylabel(f"Objective (MA={window_pct}*n_steps)")
        axes[0, 0].set_title("Objective")
        axes[0, 0].legend()

        sns.lineplot(
            data=subset,
            x="step",
            y="unsat_ma",
            hue=hue,
            palette="husl",
            alpha=0.7,
            errorbar=errorbar,
            ax=axes[0, 1],
        )
        axes[0, 1].set_xlabel("Step")
        axes[0, 1].set_ylabel(f"Unsat (MA={window_pct}*n_steps)")
        axes[0, 1].set_title("Unsatisfied Clauses")
        axes[0, 1].legend().remove()

        # zoomed in
        subset_zoom = subset[subset["step"] <= steps].copy()
        window_zoom = max(1, int(steps * window_pct))
        subset_zoom["objective_ma_zoom"] = subset_zoom.groupby(hue)[
            "objective"
        ].transform(lambda x: x.rolling(window=window_zoom, min_periods=1).mean())
        subset_zoom["unsat_ma_zoom"] = subset_zoom.groupby(hue)["unsat"].transform(
            lambda x: x.rolling(window=window_zoom, min_periods=1).mean()
        )
        sns.lineplot(
            data=subset_zoom,
            x="step",
            y="objective_ma_zoom",
            hue=hue,
            palette="husl",
            alpha=0.7,
            errorbar=errorbar,
            ax=axes[1, 0],
            legend=False,
        )
        axes[1, 0].axhline(
            optimal, color="blue", linestyle="--", label=f"Optimal ({optimal})"
        )
        axes[1, 0].set_xlabel("Step")
        axes[1, 0].set_ylabel(f"Objective (MA={window_pct}*n_steps)")
        axes[1, 0].set_title(f"Objective (first {steps} steps)")
        axes[1, 0].legend()

        sns.lineplot(
            data=subset_zoom,
            x="step",
            y="unsat_ma_zoom",
            hue=hue,
            palette="husl",
            alpha=0.7,
            errorbar=errorbar,
            ax=axes[1, 1],
        )
        axes[1, 1].set_xlabel("Step")
        axes[1, 1].set_ylabel(f"Unsat (MA={window_pct}*n_steps)")
        axes[1, 1].set_title(f"Unsatisfied Clauses (first {steps} steps)")

        handles, labels = axes[1, 1].get_legend_handles_labels()
        axes[1, 1].legend().remove()
        fig.legend(handles, labels, loc="center right", bbox_to_anchor=(1.0, 0.5))

        fig.tight_layout(rect=[0, 0, 0.9, 1])
        return fig
    return (plot_convergence_runs,)


@app.cell
def _(REPORT_PATH, conv_df, plot_convergence_runs):
    instance = conv_df["instance"].unique()[0]
    fig_runs = plot_convergence_runs(conv_df, instance)
    fig_runs.savefig(REPORT_PATH / "convergence_runs.pdf", bbox_inches="tight")
    fig_runs
    return


@app.cell
def _():
    return


if __name__ == "__main__":
    app.run()
