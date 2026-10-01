# Copyright (c) 2026 AIperture-Labs & Xavier Beheydt <xavier.beheydt@gmail.com>
# Doc: https://just.systems/man/en/

rmdir := if os_family() == "windows" { "rm -Force" } else { "rm -fr" }
install := "paru -S"
uninstall := "paru -Rs"
deps := "llvm lldb vulkan-devel lib32-nvidia-utils renderdoc glm ninja clang volk sdl3"


[default]
help:
    @just --list

[linux]
install-deps:
    {{install}} {{deps}}

# Uninstall deps, keeping those still required by other installed packages
[linux]
uninstall-deps:
    #!/usr/bin/env bash
    set -euo pipefail
    export LC_ALL=C

    # Expand groups (e.g. vulkan-devel) and keep only installed packages
    declare -A candidates=()
    for dep in {{deps}}; do
        if pacman -Qq "$dep" &>/dev/null; then
            candidates[$dep]=1
        else
            for pkg in $(pacman -Qgq "$dep" 2>/dev/null || true); do
                candidates[$pkg]=1
            done
        fi
    done

    # Drop packages required by something outside the candidates, until stable
    changed=1
    while (( changed )); do
        changed=0
        for pkg in "${!candidates[@]}"; do
            for req in $(pacman -Qi "$pkg" | sed -n 's/^Required By *: //p'); do
                if [[ $req != None && -z ${candidates[$req]:-} ]]; then
                    echo "Keeping $pkg (required by $req)"
                    unset "candidates[$pkg]"
                    changed=1
                    break
                fi
            done
        done
    done

    if (( ${#candidates[@]} == 0 )); then
        echo "Nothing to uninstall."
        exit 0
    fi
    {{uninstall}} "${!candidates[@]}"

build:
    cmake --preset debug
    cmake --build --preset debug -j

clean-build:
    {{rmdir}} build

run: build
    ./build/debug/bin/how-to-vulkan

debug: build
    lldb ./build/debug/bin/how-to-vulkan
