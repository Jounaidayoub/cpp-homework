import csv
import sys
from pathlib import Path

import matplotlib.pyplot as plt


def read_rows(path):
    with path.open(newline="") as stream:
        return list(csv.DictReader(stream))


def average_rows(rows):
    groups = {}
    for row in rows:
        key = (row["structure"], row["n"])
        group = groups.setdefault(key, {"structure": row["structure"], "n": int(row["n"]), "runs": 0})
        group["runs"] += 1
        for field in ["build_push_us", "build_heapify_us", "top_all_us", "drain_us", "release_us"]:
            group[field] = group.get(field, 0.0) + float(row[field])
        group["drain_count"] = row["drain_count"]
        group["capacity_before_release"] = row["capacity_before_release"]
        group["order_ok"] = row["order_ok"]
    for group in groups.values():
        for field in ["build_push_us", "build_heapify_us", "top_all_us", "drain_us", "release_us"]:
            group[field] /= group["runs"]
    return sorted(groups.values(), key=lambda group: (group["n"], group["structure"]))


def save_averages(rows, path):
    fields = ["structure", "n", "runs", "build_push_us", "build_heapify_us", "top_all_us", "drain_us", "release_us", "drain_count", "capacity_before_release", "order_ok"]
    with path.open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def plot_metric(rows, metrics, title, filename, output):
    structures = sorted({row["structure"] for row in rows})
    sizes = sorted({row["n"] for row in rows})
    colors = {"manual_vector": "#2563eb", "std_priority_queue": "#dc2626"}
    styles = {"build_push_us": "-", "build_heapify_us": "--", "top_all_us": "-", "drain_us": "--", "release_us": ":"}
    labels = {
        "build_push_us": "construction par push",
        "build_heapify_us": "construction par heapify",
        "top_all_us": "top() répété",
        "drain_us": "vidage par pop",
        "release_us": "libération du stockage",
    }
    figure, axis = plt.subplots(figsize=(8, 4.6))
    for structure in structures:
        selected = [row for row in rows if row["structure"] == structure]
        selected.sort(key=lambda row: row["n"])
        for metric in metrics:
            axis.plot(
                [row["n"] for row in selected],
                [row[metric] / 1000 for row in selected],
                marker="o",
                linestyle=styles[metric],
                color=colors[structure],
                label=f"{structure} — {labels[metric]}",
            )
    axis.set_title(title)
    axis.set_xlabel("Nombre d'éléments")
    axis.set_ylabel("Temps moyen (ms)")
    axis.set_xticks(sizes)
    axis.set_xticklabels([f"{size:,}".replace(",", " ") for size in sizes])
    axis.grid(True, alpha=0.25)
    axis.legend(fontsize=8)
    figure.tight_layout()
    figure.savefig(output / filename, dpi=180)
    plt.close(figure)


def main():
    input_path = Path(sys.argv[1])
    output = Path(sys.argv[2])
    output.mkdir(parents=True, exist_ok=True)
    rows = average_rows(read_rows(input_path))
    save_averages(rows, output / "benchmark-averages.csv")
    plot_metric(rows, ["build_push_us", "build_heapify_us"], "Construction du tas", "build.png", output)
    plot_metric(rows, ["top_all_us", "drain_us"], "Accès au maximum et vidage", "operations.png", output)
    plot_metric(rows, ["release_us"], "Libération du stockage après vidage", "release.png", output)


if __name__ == "__main__":
    main()
