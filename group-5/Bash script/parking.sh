#!/bin/bash

echo "Starting parking simulation..."

gnome-terminal --tab --working-directory="/opt/odv/bin" --title="Supercomponent" -e "bash -c './supercomponent --cid=111 --freq=25';bash" --tab --working-directory="/opt/odv/bin" --title="Cockpit" -e "bash -c './cockpit --cid=111';bash" --tab --working-directory="/opt/odv/bin" --title="Vehicle" -e "bash -c './vehicle --cid=111 --freq=25';bash" --tab --working-directory="/opt/odv/bin" --title="Irus" -e "bash -c './irus --cid=111 --freq=25';bash" --tab --working-directory="/opt/odv/bin" --title="Driver" -e "bash -c './driver --cid=111 --freq=25';bash"
