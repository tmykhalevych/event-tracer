import sys
import webbrowser

import plotly.express as px

from pandas import DataFrame, to_datetime
from event import Event as FreertosEvent

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
        self._start_event: FreertosEvent = None
        self._stop_event: FreertosEvent = None

    def process(self, event: FreertosEvent):
        if event is None:
            return
        
        if event.id is FreertosEvent.Id.TASK_SWITCHED_IN:
            self._process_switched_in(event)
            return

        if event.id is FreertosEvent.Id.USER_START_CAPTURING:
            self._start_event = event
            self._report_name = event.text
            self._capturing = True
        elif event.id is FreertosEvent.Id.USER_STOP_CAPTURING:
            self._stop_event = event
            self._display_report()
            self._capturing = False
        elif event.id is FreertosEvent.Id.DUMP_SYSTEM_STATE:
            self._task_names[event.task] = event.text
        elif event.id is FreertosEvent.Id.TASK_CREATE:
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
        for event in task_events:
            if event.task in self._task_names:
                event.text = self._task_names[event.task]
            else:
                event.text = f"Task #{event.task}"

        task_create_events = extract_by(FreertosEvent.Id.TASK_CREATE)
        task_delete_events = extract_by(FreertosEvent.Id.TASK_DELETE)
        messages = extract_by(FreertosEvent.Id.USER_MESSAGE)

        df = DataFrame(task_events)
        df['end'] = to_datetime(df['ts'] + df['length'], unit='us')
        df['ts'] = to_datetime(df['ts'], unit='us')

        # draw tasks execution periods
        gantt_fig = px.timeline(df,
                                x_start='ts',
                                x_end='end',
                                y='text',
                                color='prio',
                                title='[perf-tools] Tasks execution sequence')
  
        gantt_fig.update_layout(xaxis_title='Timeline [us]',
                                yaxis_title='Task',
                                coloraxis_colorbar=dict(title='Task priority'),
                                showlegend=True)

        gantt_fig.update_xaxes(dtick=100,
                               tickformat='%H:%M:%S.%L',
                               minor=dict(dtick=10, griddash='dot'))

        # draw context swith points
        scatter_fig = px.scatter(df, x='ts', y='text', color='prio', size_max=10)
        gantt_fig.add_trace(scatter_fig.data[0])

        # draw messages
        for message in messages:
            timepoint = to_datetime(message.ts, unit='us')
            gantt_fig.add_vline(x=timepoint, line_width=1, line_dash="dash", line_color="green")
            gantt_fig.add_annotation(x=timepoint, text=message.text)

        # draw doundaries
        startpoint = to_datetime(self._start_event.ts, unit='us')
        gantt_fig.add_vline(x=startpoint, line_width=1, line_dash="dash", line_color="red")
        annotation = 'start capturing'
        if self._start_event.text: annotation = f'start {self._start_event.text} capturing'
        gantt_fig.add_annotation(x=startpoint, text=annotation)

        endtpoint = to_datetime(self._stop_event.ts, unit='us')
        gantt_fig.add_vline(x=endtpoint, line_width=1, line_dash="dash", line_color="red")
        gantt_fig.add_annotation(x=endtpoint, text="stop capturing")

        # save and show the report
        report = f"{self._out_dir}/{self._report_name}.html"

        gantt_fig.write_html(report)
        webbrowser.open('file://' + report)

def main():
    app = TasksExecutionVisualizer()
    for line in sys.stdin.buffer.raw: app.process(FreertosEvent.parse(line))

if __name__ == "__main__":
    main()
