import re

from dataclasses import dataclass, asdict
from enum import Enum


@dataclass
class Event:
    # event format: [ts|event_id|task|info_id:info]
    PATTERN = r"\[([0-9]*)\|([0-9]*)\|([0-9]*)\|([0-9]*)\:(.*)\]"

    class Id(Enum):
        DUMP_SYSTEM_STATE = 1
        USER_START_CAPTURING = 2
        USER_STOP_CAPTURING = 3
        USER_MESSAGE = 4
        TASK_CREATE = 7
        TASK_DELETE = 11
        TASK_SWITCHED_IN = 26

    class InfoId(Enum):
        UNDEFINED = 0
        TASK_PRIO = 1
        MESSAGE = 2
        MARKER = 3

    ts: int
    id: Id
    task: int

    length: int = None
    prio: int = None
    mark: int = None

    text: str = None

    def __str__(self) -> str:
        return str(asdict(self))

    def parse(input: bytes) -> "Event":
        res = re.search(Event.PATTERN, str(input).strip())

        if not res:
            return None

        id = int(res.group(2))
        try:
            id = Event.Id(id)
        except ValueError:
            return None

        info_id = int(res.group(4))
        try:
            info_id = Event.InfoId(info_id)
        except ValueError:
            return None

        ts = int(res.group(1))
        task = int(res.group(3))

        event = Event(ts, id, task)

        if info_id is Event.InfoId.UNDEFINED:
            return event

        info = res.group(5)

        if info_id is Event.InfoId.TASK_PRIO:
            event.prio = int(info)
        if info_id is Event.InfoId.MESSAGE:
            event.text = str(info)
        if info_id is Event.InfoId.MARKER:
            event.mark = int(info)

        return event
