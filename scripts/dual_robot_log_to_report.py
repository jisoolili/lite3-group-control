#!/usr/bin/env python3
"""把 dual_robot_demo / lite3_stand_demo 日志整理成测试报告表格行。

脚本只读取已有日志文件，不连接机器人，不发送控制命令。
"""

from __future__ import annotations

import argparse
import datetime as dt
import pathlib
import re
import sys


TIMING_RE = re.compile(
    r"\[TIMING\]\s+robot=(?P<robot>\S+)\s+event=(?P<event>\S+).*?(?:elapsed_ms|since_t0_ms)=(?P<ms>[0-9.]+)"
)
ACTION_RE = re.compile(r"^\[(?P<robot>Robot_[AB])\]\s+action=(?P<action>\S+)")
REAL_ACTION_RE = re.compile(r"^action=(?P<action>\S+)$")
DELAY_RE = re.compile(r"^delay=(?P<delay>[0-9.]+)$")
MODE_RE = re.compile(r"^mode=(?P<mode>\S+)$")
EXIT_RE = re.compile(r"(?:\[EXIT\].*?code=|exit_code=)(?P<code>-?[0-9]+)")


def cell(value: str | None) -> str:
    if value is None or value == "":
        return "-"
    return value.replace("|", "\\|").replace("\n", " ")


def read_log(path: pathlib.Path) -> list[str]:
    return path.read_text(encoding="utf-8", errors="replace").splitlines()


def parse_log(lines: list[str]) -> dict[str, object]:
    info: dict[str, object] = {
        "mode": "",
        "delay": "",
        "exit_codes": [],
        "errors": [],
        "actions": {"Robot_A": "", "Robot_B": ""},
        "timing": {"Robot_A": {}, "Robot_B": {}},
    }

    current_real_robot = ""
    for line in lines:
        stripped = line.strip()
        mode_match = MODE_RE.match(stripped)
        if mode_match:
            info["mode"] = mode_match.group("mode")

        if stripped.startswith("robot=Robot_"):
            current_real_robot = stripped.split("=", 1)[1]

        action_match = ACTION_RE.match(stripped)
        if action_match:
            info["actions"][action_match.group("robot")] = action_match.group("action")  # type: ignore[index]

        real_action_match = REAL_ACTION_RE.match(stripped)
        if real_action_match and current_real_robot in ("Robot_A", "Robot_B"):
            info["actions"][current_real_robot] = real_action_match.group("action")  # type: ignore[index]

        delay_match = DELAY_RE.match(stripped)
        if delay_match:
            info["delay"] = delay_match.group("delay")

        timing_match = TIMING_RE.search(stripped)
        if timing_match:
            robot = timing_match.group("robot")
            event = timing_match.group("event")
            info["timing"][robot][event] = timing_match.group("ms")  # type: ignore[index]

        exit_match = EXIT_RE.search(stripped)
        if exit_match:
            info["exit_codes"].append(exit_match.group("code"))  # type: ignore[index]

        if "ERROR:" in stripped or "exec_failed" in stripped or "[EXIT]" in stripped:
            info["errors"].append(stripped)  # type: ignore[index]

    return info


def pick_time(timing: dict[str, str], *events: str) -> str:
    for event in events:
        if event in timing:
            return f"{timing[event]} ms"
    return "-"


def build_row(args: argparse.Namespace, parsed: dict[str, object]) -> str:
    actions = parsed["actions"]  # type: ignore[assignment]
    timing = parsed["timing"]  # type: ignore[assignment]
    exit_codes = parsed["exit_codes"]  # type: ignore[assignment]
    errors = parsed["errors"]  # type: ignore[assignment]

    robot_a_timing = timing["Robot_A"]
    robot_b_timing = timing["Robot_B"]

    exit_code = args.exit_code or (",".join(exit_codes) if exit_codes else "-")
    error_text = args.error or ("; ".join(errors[:3]) if errors else "-")
    test_time = args.test_time or dt.datetime.fromtimestamp(
        pathlib.Path(args.log).stat().st_mtime
    ).strftime("%Y-%m-%d %H:%M:%S")

    columns = [
        args.test_id,
        test_time,
        args.project,
        args.robot_a_action or actions["Robot_A"],
        args.robot_b_action or actions["Robot_B"],
        args.delay or parsed["delay"],
        args.mode or parsed["mode"],
        pick_time(robot_a_timing, "T1_CHILD_CREATED", "T2_LITE3_STAND_DEMO_START"),
        pick_time(robot_b_timing, "T1_CHILD_CREATED", "T2_LITE3_STAND_DEMO_START"),
        pick_time(robot_a_timing, "T8_STAND_UP_BEGIN"),
        pick_time(robot_b_timing, "T8_STAND_UP_BEGIN"),
        pick_time(robot_a_timing, "T_ACTION_DONE", "T_CONTROL_DONE"),
        pick_time(robot_b_timing, "T_ACTION_DONE", "T_CONTROL_DONE"),
        args.result,
        exit_code,
        error_text,
        args.video,
        args.notes,
    ]
    return "| " + " | ".join(cell(str(column)) for column in columns) + " |"


def main() -> int:
    parser = argparse.ArgumentParser(
        description="从双机器人测试日志生成 docs/dual-robot-test-report.md 表格行。"
    )
    parser.add_argument("--log", required=True, help="测试日志文件")
    parser.add_argument("--test-id", required=True, help="测试编号，例如 TC-01")
    parser.add_argument("--project", required=True, help="测试项目")
    parser.add_argument(
        "--result",
        required=True,
        choices=("PASS", "FAIL", "BLOCKED", "PENDING"),
        help="实际执行结果",
    )
    parser.add_argument("--test-time", default="", help="测试时间，默认使用日志 mtime")
    parser.add_argument("--robot-a-action", default="", help="手动覆盖 Robot_A 动作")
    parser.add_argument("--robot-b-action", default="", help="手动覆盖 Robot_B 动作")
    parser.add_argument("--delay", default="", help="手动覆盖 delay")
    parser.add_argument("--mode", default="", help="手动覆盖执行模式")
    parser.add_argument("--exit-code", default="", help="手动覆盖退出码")
    parser.add_argument("--error", default="", help="手动覆盖异常信息")
    parser.add_argument("--video", default="-", help="视频文件名")
    parser.add_argument("--notes", default="-", help="备注")
    args = parser.parse_args()

    log_path = pathlib.Path(args.log)
    if not log_path.is_file():
        print(f"ERROR: log file not found: {log_path}", file=sys.stderr)
        return 2

    parsed = parse_log(read_log(log_path))
    print(build_row(args, parsed))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

