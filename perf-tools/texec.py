import sys
import signal
import argparse
import matplotlib.pyplot as plt

from matplotlib.animation import FuncAnimation
from event import *

class TasksExecutionVisualizer:
    def __init__(self):
        self._configure()
        self._start()

    def process(self, events: list) -> None:
        for event in events:
            print(event)

    def _configure(self) -> None:
        self._figure, self._ax = plt.subplots(figsize=(10, 6))
        self._ax.set_xlabel('Timeline')
        self._ax.set_title('[perf-tools] Tasks execution sequence')
        self._animation = FuncAnimation(self._figure, lambda frames: self._update_plot(frames), interval=1000)

    def _update_plot(self, frames) -> None:
        # TODO: add new events
        pass

    def _start(self) -> None:
        plt.show()

def main():
    signal.signal(signal.SIGINT, lambda *_: sys.exit(0))

    visualizer = TasksExecutionVisualizer()
    event_registry = EventRegistry(lambda events: visualizer.process(events))

    for line in sys.stdin:
        event_registry.push(Event.parse(line))

if __name__ == "__main__":
    main()
