import marimo

__generated_with = "0.17.8"
app = marimo.App(width="medium")


@app.cell
def _():
    import marimo as mo
    import matplotlib.pyplot as plt
    import numpy as np
    import pandas as pd
    import seaborn as sns
    from pathlib import Path

    ROOT = Path(__file__).parent
    PLOT_PATH = ROOT / 'data' / 'plots'


    plt.style.use("ggplot")
    plt.rcParams["figure.facecolor"] = "white"
    plt.rcParams["axes.facecolor"] = "white"

    plt.rcParams["text.color"] = "black"
    plt.rcParams["axes.labelcolor"] = "black"
    plt.rcParams["xtick.color"] = "black"
    plt.rcParams["ytick.color"] = "black"
    return PLOT_PATH, mo, np, pd, plt, sns


@app.cell
def _instance_to_dataset(pd):
    def instance_to_dataset(x: str):
        return "-".join(x.split("-")[:2])


    def to_latex(
        pivot: pd.DataFrame, func: str | None = "min", format_string: str = "%.2f"
    ) -> str:
        def bold_extreme_values(data, format_string=format_string, func=func):
            if func == "max":
                extrema = data != data.max()
            else:
                extrema = data != data.min()
            bolded = data.apply(lambda x: "\\textbf{%s}" % (format_string % x))
            formatted = data.apply(lambda x: format_string % x)
            return formatted.where(extrema, bolded)

        highlighted = pivot
        if func:
            highlighted = pivot.apply(bold_extreme_values, axis=1)
        return highlighted.to_latex(escape=False)
    return instance_to_dataset, to_latex


@app.cell(hide_code=True)
def _(mo):
    mo.md(r"""
    # Run-specific stats
    """)
    return


@app.cell(hide_code=True)
def _(instance_to_dataset, np, pd):
    df = pd.read_parquet("data/data.parquet", engine="pyarrow").replace(
        {"gsat2-distr/gsat2": "gsat", "probSAT/probSAT": "probsat"}
    )
    df["solved"] = df["satisfied_c"] == df["total_c"]
    df["dataset"] = df["instance"].map(instance_to_dataset)
    df["penalized_flips"] = df["flips"] + ~df["solved"] * 9 * df["flips"]
    df["log_flips"] = df["flips"].apply(np.log1p)
    datasets = sorted(df["dataset"].unique())
    df_solved = df[df.solved]
    df
    return datasets, df, df_solved


@app.cell(hide_code=True)
def _(mo):
    mo.md(r"""
    # Instance-specific stats
    """)
    return


@app.cell(hide_code=True)
def _(mo):
    mo.md(r"""
    ## Success rate
    """)
    return


@app.cell(hide_code=True)
def _(df, to_latex):
    succ_rate = (
        df.pivot_table(
            columns="algorithm", index="dataset", aggfunc="mean", values="solved"
        )
        * 100
    ).round(2)
    print(to_latex(succ_rate, "max"))
    succ_rate
    return


@app.cell(hide_code=True)
def _(mo):
    mo.md(r"""
    ## Penalized average runtime (PAR10)
    """)
    return


@app.cell(hide_code=True)
def _(df, to_latex):
    par10 = df.pivot_table(
        columns="algorithm",
        index="dataset",
        aggfunc="mean",
        values="penalized_flips",
    ).round()
    print(to_latex(par10, "min"))
    par10
    return


@app.cell
def _(mo):
    mo.md(r"""
    ## CDF
    """)
    return


@app.cell
def _(PLOT_PATH, datasets, df_solved, sns):
    cdf = (
        sns.FacetGrid(
            df_solved, col="dataset", col_order=datasets, height=4, hue="algorithm"
        )
        .map(sns.ecdfplot, "flips")
        .add_legend()
    )
    cdf.savefig(PLOT_PATH / 'cdf.pdf')
    cdf
    return


@app.cell
def _(PLOT_PATH, datasets, df, np, plt, sns):


    def penalized_ecdf(data, x, **kws):
        t = data[x].to_numpy()
        solved = data["solved"]

        t_solved = np.sort(t[solved])
        if len(t_solved) == 0:
            return

        rho = solved.mean()
        y = np.arange(1, len(t_solved) + 1) / len(t_solved) * rho

        plt.step(t_solved, y, where="post", **kws)


    cdf_corrected = (
        sns.FacetGrid(
            df, col="dataset", col_order=datasets, height=4, hue="algorithm"
        )
        .map_dataframe(penalized_ecdf, x="flips")
        .add_legend()
    )

    cdf_corrected.savefig(PLOT_PATH / 'cdf_corrected.pdf')

    cdf_corrected
    return


@app.cell
def _(mo):
    bin_count = mo.ui.number(25, 150)
    bin_count
    return (bin_count,)


@app.cell
def _(PLOT_PATH, bin_count, datasets, df, sns):
    hist = (
        sns.FacetGrid(
            df[df.solved],
            col="dataset",
            col_order=datasets,
            height=4,
            hue="algorithm",
        )
        .map(sns.histplot, "flips", bins=bin_count.value)
        .add_legend()
    )
    hist.savefig(PLOT_PATH / 'hist.pdf')
    hist
    return


@app.cell
def _(PLOT_PATH, bin_count, datasets, df, sns):
    log_hist = (
        sns.FacetGrid(
            df[df.solved],
            col="dataset",
            col_order=datasets,
            height=4,
            hue="algorithm",
        )
        .map(sns.histplot, "log_flips", bins=bin_count.value)
        .add_legend()
    )

    log_hist.savefig(PLOT_PATH / 'log_hist.pdf')
    log_hist
    return


@app.cell
def _(df, to_latex):
    log_flips_mean = (
        df.pivot_table(
            columns="algorithm",
            index="dataset",
            aggfunc="mean",
            values="log_flips",
        )
    ).round(2)
    print(to_latex(log_flips_mean, "min"))
    log_flips_mean
    return


@app.cell
def _(df, to_latex):
    log_flips_std = (
        df.pivot_table(
            columns="algorithm",
            index="dataset",
            aggfunc="std",
            values="log_flips",
        )
    ).round(2)
    print(to_latex(log_flips_std))
    log_flips_std
    return


@app.cell
def _():
    return


if __name__ == "__main__":
    app.run()
