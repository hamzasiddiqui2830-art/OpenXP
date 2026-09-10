"""Apply the active OpenXP WRK v1.2/SP0 compatibility edits.

The implementation lives in fix_wrk_mm_ci.py. This small entry point makes the
intended CI/build invocation explicit without ever touching ntos-old.
"""

from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "scripts" / "fix_wrk_mm_ci.py"

if not SCRIPT.is_file():
    raise SystemExit(f"missing compatibility script: {SCRIPT}")

subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, check=True)
