from pathlib import Path
import json
import subprocess

Import("env", "pio_lib_builder")

project_dir = Path(env.subst("$PROJECT_DIR"))
environment_name = env.subst("$PIOENV")
libdeps_dir = project_dir / ".pio" / "libdeps" / environment_name
bplc_hal_dir = Path(pio_lib_builder.path).resolve()


def get_commit(repository_dir):
    try:
        result = subprocess.run(
            ["git", "-C", str(repository_dir), "rev-parse", "HEAD"],
            check=True,
            capture_output=True,
            text=True,
        )
    except (OSError, subprocess.CalledProcessError):
        return "unknown"

    commit = result.stdout.strip()
    return commit if commit else "unknown"


def get_version(repository_dir):
    try:
        with (repository_dir / "library.json").open(encoding="utf-8") as metadata:
            version = json.load(metadata).get("version")
    except (OSError, json.JSONDecodeError, AttributeError):
        return "unknown"

    return version if isinstance(version, str) and version else "unknown"


def get_standartlibs_dir():
    extra_dirs = env.GetProjectOption("lib_extra_dirs", [])
    if isinstance(extra_dirs, str):
        extra_dirs = extra_dirs.splitlines()

    for extra_dir in extra_dirs:
        candidate = Path(env.subst(extra_dir.strip())).resolve()
        if candidate.name.casefold() == "standartlibs":
            return candidate

    return project_dir.parent / "Standartlibs"


metadata_dir = Path(env.subst("$BUILD_DIR")) / "generated" / "bplc_hal"
metadata_dir.mkdir(parents=True, exist_ok=True)
metadata_file = metadata_dir / "build_metadata.h"

metadata_lines = [
    "#pragma once",
    "",
    f'#define BPLC_HAL_VERSION "{get_version(bplc_hal_dir)}"',
    f'#define BERTANETPORTS_VERSION "{get_version(libdeps_dir / "BertaNetPorts")}"',
    f'#define BPLC_HAL_COMMIT "{get_commit(bplc_hal_dir)}"',
    f'#define BERTANETPORTS_COMMIT "{get_commit(libdeps_dir / "BertaNetPorts")}"',
    f'#define STANDARTLIBS_COMMIT "{get_commit(get_standartlibs_dir())}"',
    "",
]
metadata_file.write_text("\n".join(metadata_lines), encoding="ascii")
env.Prepend(CPPPATH=[str(metadata_dir)])
print(f"Generated BPLC_HAL dependency metadata: {metadata_file}")
