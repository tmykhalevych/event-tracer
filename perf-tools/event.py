import json

from datetime import timedelta
from dataclasses import dataclass, asdict
from enum import Enum

@dataclass
class Event:
    class Id(Enum):
        USER = 1
        TASK_CREATE = 4
        TASK_DELETE = 8
        TASK_SWITCHED_IN = 23

    ts_start: int
    id: Id
    task: int

    ts_end: int = None
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

        ts_start = data["ts"]
        task = data["ctx"]["task"]

        event = Event(ts_start, id, task)

        info = data["ctx"]["info"]
        if "prio" in info:
            event.prio = info["prio"]
        if "msg" in info:
            event.msg = info["msg"]
        if "mark" in info:
            event.mark = info["mark"]

        return event
