import json
from dataclasses import dataclass, asdict
from enum import Enum
from typing import Callable

@dataclass
class Event:
    class Id(Enum):
        USER = 1
        TASK_CREATE = 4
        TASK_DELETE = 8
        TASK_SWITCHED_IN = 23

    ts: int # us
    id: Id
    task: int
    prio: int = None
    msg: str = None
    mark: int = None

    def __str__(self) -> str:
        return str(asdict(self))

    def parse(input: str) -> 'Event':
        data = None
        try:
            data = json.loads(input)
        except json.JSONDecodeError:
            return None

        id = None
        try:
            id = Event.Id(data["event"])
        except ValueError:
            return None

        ts = data["ts"]
        task = data["ctx"]["task"]

        event = Event(ts, id, task)

        info = data["ctx"]["info"]
        if "prio" in info:
            event.prio = info["prio"]
        if "msg" in info:
            event.msg = info["msg"]
        if "mark" in info:
            event.mark = info["mark"]

        return event
    
class EventRegistry:
    def __init__(self, event_processor: Callable[[list], None], capacity_ms: int = 1000):
        self._events = []
        self._last_event_ts = None
        self._event_processing_window_ms = capacity_ms
        self._event_processor = event_processor

    def push(self, event: Event) -> None:
        if event is None:
            return

        self._events.append(event)
    
        if self._last_event_ts is None:
            self._last_event_ts = event.ts
            return

        if (event.ts - self._last_event_ts) >= (self._event_processing_window_ms * 1000):
            self._event_processor(self._events)
            self._events.clear()
