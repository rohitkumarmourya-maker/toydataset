import pandas as pd
import matplotlib.pyplot as plt

# Load experimental results
data = pd.read_csv("pf_results.csv")

# Separate based on replacement policy
df_lru = data.loc[data["policy"] == 1]
df_mru = data.loc[data["policy"] == 2]

# (column, display name)
metrics = [
    ("logicalReads", "Logical Reads"),
    ("logicalWrites", "Logical Writes"),
    ("physicalReads", "Physical Reads"),
    ("physicalWrites", "Physical Writes"),
    ("cacheHits", "Cache Hits"),
    ("cacheMisses", "Cache Misses")
]

# Prepare figure layout
fig, axs = plt.subplots(nrows=3, ncols=2, figsize=(14, 12))
axs = axs.ravel()

for idx, (col, title) in enumerate(metrics):
    ax = axs[idx]

    ax.plot(df_lru["writePct"], df_lru[col], label="LRU", marker='o')
    ax.plot(df_mru["writePct"], df_mru[col], label="MRU", marker='o')

    ax.set_title(title)
    ax.set_xlabel("Write %")
    ax.set_ylabel(title)
    ax.legend()
    ax.grid(True)

fig.tight_layout()
fig.savefig("combined_pf_plot.png", dpi=300)
plt.show()

print("Plot stored as combined_pf_plot.png")
