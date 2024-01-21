import sys
import webbrowser

import plotly.express as px

from datetime import datetime
from pandas import DataFrame, to_datetime
from event import Event as FreertosEvent

class TasksExecutionVisualizer:
    def __init__(self, out_dir: str = '.', preroll_ms: int = 100, postroll_ms: int = 100, show: bool = False):
        self._out_dir = out_dir
        self._preroll_ms = preroll_ms
        self._postroll_ms = postroll_ms
        self._show_report = show

        self._events = []
        self._task_names = {}

        self._capturing = False
        self._report_info = {}
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
            self._report_info = dict(time=datetime.now(), name=event.text)
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
        extract_by = lambda id: [event for event in self._events if event.id is id]

        # draw tasks execution periods
        task_events = extract_by(FreertosEvent.Id.TASK_SWITCHED_IN)
        for event in task_events:
            if event.task in self._task_names:
                event.text = self._task_names[event.task]
            else:
                event.text = f'Task #{event.task}'

        task_events_df = DataFrame(task_events)
        task_events_df['end'] = to_datetime(task_events_df['ts'] + task_events_df['length'], unit='us')
        task_events_df['ts'] = to_datetime(task_events_df['ts'], unit='us')

        main_fig = px.timeline(task_events_df,
                                x_start='ts',
                                x_end='end',
                                y='text',
                                color='prio',
                                title='[perf-tools] Tasks execution sequence')
  
        main_fig.update_layout(xaxis_title='Timeline [us]',
                                yaxis_title='Task',
                                coloraxis_colorbar=dict(title='Task priority'),
                                showlegend=True)

        main_fig.update_xaxes(dtick=100,
                               tickformat='%H:%M:%S.%L',
                               minor=dict(dtick=10, griddash='dot'))

        # draw context swith points
        ctx_switch_fig = px.scatter(task_events_df, x='ts', y='text', color='prio', size_max=10)
        main_fig.add_trace(ctx_switch_fig.data[0])

        # draw tasks lifetime events
        task_lifetime_events = extract_by(FreertosEvent.Id.TASK_CREATE) + extract_by(FreertosEvent.Id.TASK_DELETE)
        task_lt_events_df = DataFrame(task_lifetime_events)
        task_lt_events_df['ts'] = to_datetime(task_lt_events_df['ts'], unit='us')
        lifetimes_fig = px.scatter(task_lt_events_df, x='ts', y='text', size_max=10, color_discrete_sequence=['black'])

        main_fig.add_trace(lifetimes_fig.data[0])

        # draw messages
        messages = extract_by(FreertosEvent.Id.USER_MESSAGE)
        for message in messages:
            timepoint = to_datetime(message.ts, unit='us')
            main_fig.add_vline(x=timepoint, line_width=1, line_dash='dash', line_color='green')
            main_fig.add_annotation(x=timepoint, text=message.text)

        # draw doundaries
        startpoint = to_datetime(self._start_event.ts, unit='us')
        main_fig.add_vline(x=startpoint, line_width=1, line_dash='dash', line_color='red')
        main_fig.add_annotation(x=startpoint, text=f'start capturing {self._start_event.text}'.strip())

        endtpoint = to_datetime(self._stop_event.ts, unit='us')
        main_fig.add_vline(x=endtpoint, line_width=1, line_dash='dash', line_color='red')
        main_fig.add_annotation(x=endtpoint, text='stop capturing')

        # save report
        name = self._report_info['name']
        time = self._report_info['time'].strftime('%d-%m-%Y__%H-%M-%S')
        report = f'{self._out_dir}/texec__{name}__{time}.html'

        main_fig.write_html(report)

        # show report
        if self._show_report:
            webbrowser.open('file://' + report)

def main():
    app = TasksExecutionVisualizer()
    for line in sys.stdin.buffer.raw: app.process(FreertosEvent.parse(line))

if __name__ == '__main__':
    main()
