import sys
import signal
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

from threading import Thread, Event as ThreadEvent, Lock
from matplotlib.animation import FuncAnimation
from event import Event as FreertosEvent
from screen import *

class TasksExecutionVisualizer:
    def __init__(self, realtime: bool = True, out_dir: str = ".", update_interval_ms: int = 1000):
        self._realtime = realtime
        self._out_dir = out_dir
        self._update_interval_ms = update_interval_ms

        self._events_mtx = Lock()
        self._last_swithed_in: FreertosEvent = None
        self._tasks_switched_in = []
        self._tasks_created = []
        self._tasks_deleted = []
        self._user_events = []

        self._task_names = {}

        self._configure_plot()
    
    def start(self):
        if self._realtime:
            plt.show()

    def process(self, event: FreertosEvent) -> None:
        if event is None: return
        with self._events_mtx:
           self._handle_new_event(event)

    def save(self) -> None:
        if not self._realtime:
            plt.savefig(f"{self._out_dir}/texec.png")

    def _configure_plot(self) -> None:
        if self._realtime: matplotlib.use('WebAgg')
        else: matplotlib.use('Agg')

        self._figure, self._axes = plt.subplots(figsize=max_screen_size())
        self._axes.set_xlabel('Timeline')
        self._axes.set_ylabel('Tasks')
        self._axes.set_title('[perf-tools] Tasks execution sequence')
        self._animation = FuncAnimation(self._figure,
                                        lambda _: self._update_plot(),
                                        self._update_interval_ms)

    def _handle_new_event(self, event: FreertosEvent):
        if event.id is FreertosEvent.Id.TASK_CREATE:
            self._task_names[event.task] = event.msg

        if event.id is FreertosEvent.Id.TASK_SWITCHED_IN:
            if self._last_swithed_in:
                self._last_swithed_in.ts_end = event.ts_start
                self._tasks_switched_in.append(self._last_swithed_in)

            self._last_swithed_in = event
        else:
            event_buckets = {
                FreertosEvent.Id.TASK_CREATE: self._tasks_created,
                FreertosEvent.Id.TASK_DELETE: self._tasks_deleted,
                FreertosEvent.Id.USER: self._user_events
            }
            event_buckets[event.id].append(event)

    def _update_plot(self) -> None:
        targets = [
            (self._tasks_created, self._plot_task_created),
            (self._tasks_switched_in, self._plot_task_switched_in),
            (self._tasks_deleted, self._plot_task_deleted),
            (self._user_events, self._plot_user_event)
        ]

        with self._events_mtx:
            for collection, plot in targets:
                if collection:
                    for item in collection:
                        plot(item)

    def _plot_task_created(self, event: FreertosEvent):
        # TODO: visualize event
        pass

    def _plot_task_deleted(self, event: FreertosEvent):
        # TODO: visualize event
        pass

    def _plot_task_switched_in(self, event: FreertosEvent):
        self._axes.barh(y=self._task_names[event.task],
                        left=event.ts_start,
                        width=event.ts_end - event.ts_start,
                        color='blue')

    def _plot_user_event(self, event: FreertosEvent):
        # TODO: visualize event
        pass

def main():
    stop_event = ThreadEvent()
    signal.signal(signal.SIGINT, lambda *_: stop_event.set())

    visualizer = TasksExecutionVisualizer()

    def read_input(stop: ThreadEvent):
        for line in sys.stdin:
            if stop.is_set(): return
            visualizer.process(FreertosEvent.parse(line))

    input_thread = Thread(target=read_input, args=(stop_event,))

    input_thread.start()
    visualizer.start()

    input_thread.join()
    visualizer.save()

if __name__ == "__main__":
    main()
