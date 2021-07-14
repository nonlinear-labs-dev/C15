#!/bin/sh

echo "enabling and starting AP"
systemctl unmask accesspoint
systemctl enable accesspoint
systemctl start accesspoint