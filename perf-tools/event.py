import json

from datetime import timedelta
from dataclasses import dataclass, asdict
from enum import Enum

@dataclass
class Event:
    class Id(Enum):
        USER_START_CAPTURING = 1
        USER_STOP_CAPTURING = 2
        USER_MESSAGE = 3
        TASK_CREATE = 6
        TASK_DELETE = 10
        TASK_SWITCHED_IN = 25

    ts: int
    id: Id
    task: int

    length: int = None
    prio: int = None
    mark: int = None

    text: str = None

    def __str__(self) -> str:
        return str(asdict(self))

    def parse(input: str) -> 'Event':
        data = None
        try:
            data = json.loads(input)
        except json.JSONDecodeError:
            return None
        
        if not isinstance(data, dict) or not "event" in data:
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
            event.text = info["msg"]
        if "mark" in info:
            event.mark = info["mark"]

        return event
