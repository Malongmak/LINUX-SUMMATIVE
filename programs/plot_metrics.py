import matplotlib.pyplot as plt # type: ignore
import usage_reader # type: ignore

def plot_metrics(file_path):
    data = usage_reader.read_usage_file(file_path)

    timestamps = [entry[0] for entry in data]
    cpu_usages = [entry[1] for entry in data]
    mem_usages = [entry[2] for entry in data]
    net_rx = [entry[3] for entry in data]
    net_tx = [entry[4] for entry in data]

    plt.figure(figsize=(12, 6))
    plt.plot(timestamps, cpu_usages, label="CPU Usage (%)", color="blue")
    plt.plot(timestamps, mem_usages, label="Memory Usage (%)", color="green")
    plt.plot(timestamps, net_rx, label="Network RX (KB)", color="orange")
    plt.plot(timestamps, net_tx, label="Network TX (KB)", color="red")
    plt.xlabel("Time")
    plt.ylabel("Metrics")
    plt.title("System Performance Metrics")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.xticks(rotation=45)
    plt.show()

if __name__ == "__main__":
    plot_metrics("usage.txt")
