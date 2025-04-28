import sys
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap

# Function to parse input data
def parse_input():
    intervals = []
    task_periods = {}

    for line in sys.stdin:
        print(f"Processing line: {line}")  # Debug print
        line = line.strip()
        if not line:
            continue

        if line.startswith("Periods:"):
            print(f"Found Periods line: {line}")  # Debug print
            parts = line.split("Task")[1:]
            for part in parts:
                if "=" in part:
                    task_id, period = part.strip().split("=")
                    task_name = f"Task {task_id.strip()}"
                    task_periods[task_name] = int(period.strip())
            continue

        parts = line.split()
        if len(parts) < 2:
            continue

        time_str, task_desc = parts[0], ' '.join(parts[1:])
        try:
            time = int(time_str)
        except ValueError:
            continue

        intervals.append((task_desc, time, time + 1))

    print(f"Intervals: {intervals}")  # Debug print
    print(f"Task Periods: {task_periods}")  # Debug print

    return intervals, task_periods


# Merge intervals for the same task (if they are contiguous)
def merge_intervals(intervals):
    if not intervals:
        return []

    merged = [{'task': intervals[0][0], 'start': intervals[0][1], 'end': intervals[0][2]}]

    for task, start, end in intervals[1:]:
        last = merged[-1]
        if last['task'] == task and last['end'] == start:
            merged[-1]['end'] = end
        else:
            merged.append({'task': task, 'start': start, 'end': end})

    return merged

# Sorting function for tasks based on task priority (or task name)
def task_sort_key(task):
    if task == 'Idle':
        return (1, 0)
    elif task.startswith('Task '):
        try:
            num = int(task.split()[1])
            return (0, num)
        except:
            return (0, 0)
    return (0, 0)

# Function to plot the Gantt chart based on merged intervals
def plot_gantt(merged, task_periods):
    if not merged:
        print("No data to plot")
        return

    # Sort tasks based on the defined sort key
    tasks = sorted({m['task'] for m in merged}, key=task_sort_key)
    cmap = ListedColormap(plt.cm.tab10.colors + plt.cm.Set2.colors)
    task_colors = {task: cmap(i) for i, task in enumerate(tasks)}

    fig, ax = plt.subplots(figsize=(10, 6))

    # Plot each task as a horizontal bar
    for i, task in enumerate(tasks):
        task_intervals = [m for m in merged if m['task'] == task]
        for interval in task_intervals:
            ax.barh(i,
                    width=interval['end'] - interval['start'],
                    left=interval['start'],
                    color=task_colors[task],
                    edgecolor='black',
                    height=0.6)
    
    ax.set_xlim(0, 9)  # Limiting x-axis to 30 time units (adjust this as needed)
    ax.set_yticks(range(len(tasks)))
    ax.set_yticklabels(tasks)
    ax.set_xlabel("Time Units")
    ax.set_title("EDF Scheduling Gantt Chart")
    ax.grid(True, axis='x', linestyle='--', alpha=0.7)

    # Adding vertical dotted lines for task periods
    max_time = max(m['end'] for m in merged)
    for task, period in task_periods.items():
        color = task_colors.get(task, 'gray')
        for x in range(period, max_time + 1, period):
            ax.axvline(x=x, linestyle=':', color=color, alpha=0.5)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    intervals, task_periods = parse_input()
    merged = merge_intervals(intervals)
    plot_gantt(merged, task_periods)
