import sys
import webbrowser

import plotly.express as px
import plotly.graph_objects as go

from pandas import DataFrame, Timedelta, to_datetime
from event import Event as FreertosEvent, asdict

class TasksExecutionVisualizer:
    def __init__(self, out_dir: str = ".", preroll_ms: int = 100, postroll_ms: int = 100):
        self._out_dir = out_dir
        self._preroll_ms = preroll_ms
        self._postroll_ms = postroll_ms

        self._events = []
        self._task_names = {}

        self._capturing: bool = False
        self._report_name: str = None
        self._last_swithed_in: FreertosEvent = None

    def process(self, event: FreertosEvent):
        if event is None: return

        if event.id is FreertosEvent.Id.USER_START_CAPTURING:
            self._report_name = event.text
            self._capturing = True
        elif event.id is FreertosEvent.Id.USER_STOP_CAPTURING:
            self._display_report()
            self._capturing = False
        elif event.id is FreertosEvent.Id.TASK_SWITCHED_IN:
            self._process_switched_in(event)
        else:
            if event.id is FreertosEvent.Id.TASK_CREATE:
                self._task_names[event.task] = event.text

            self._events.append(event)

    def _process_switched_in(self, event: FreertosEvent):
        if self._last_swithed_in:
            self._last_swithed_in.length = event.ts - self._last_swithed_in.ts
            if self._capturing:
                self._events.append(self._last_swithed_in)

        self._last_swithed_in = event

    def _display_report(self):
        def extract_by(id: FreertosEvent.Id) -> list:
            return [event for event in self._events if event.id is id]

        task_events = extract_by(FreertosEvent.Id.TASK_SWITCHED_IN)
        for event in task_events: event.text = self._task_names[event.task]

        task_create_events = extract_by(FreertosEvent.Id.TASK_CREATE)
        task_delete_events = extract_by(FreertosEvent.Id.TASK_DELETE)
        messages = extract_by(FreertosEvent.Id.USER_MESSAGE)

        df = DataFrame(task_events)
        df['end'] = to_datetime(df['ts'] + df['length'])
        df['ts'] = to_datetime(df['ts'])

        scatter_fig = px.scatter(df, x='ts', y='text', color='prio', size_max=10)
        gantt_fig = px.timeline(df,
                                x_start='ts',
                                x_end='end',
                                y='text',
                                color='prio',
                                title='[perf-tools] Tasks execution sequence')
            
        gantt_fig.update_layout(xaxis_title='Timeline [us]',
                                yaxis_title='Task',
                                coloraxis_colorbar={'title': 'Task priority'},
                                showlegend=True)
    
        gantt_fig.add_trace(scatter_fig.data[0])

        report = f"{self._out_dir}/{self._report_name}.html"

        gantt_fig.write_html(report)
        webbrowser.open('file://' + report)

def main():
    app = TasksExecutionVisualizer()
    for line in sys.stdin: app.process(FreertosEvent.parse(line))

if __name__ == "__main__":
    main()
