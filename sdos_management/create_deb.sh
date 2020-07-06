PROG="sdos"
VER="0.1"
NAME="${PROG}_$VER"
BASE=$(dirname "$SCRIPT")
TARGET="$BASE/$NAME/usr/share/sdos"

# compile the python application to a single, statically linked executable
pyinstaller --hidden-import flask --hidden-import flask-cors --noconsole --onefile -w -F app.py
staticx dist/app dist/static-app

rm -r "${BASE:?}/${NAME:?}"
rm -f "${BASE:?}/*.deb"
mkdir "${BASE}/${NAME}"

# create application folder structure
mkdir "$BASE/$NAME/DEBIAN"
mkdir "$BASE/$NAME/usr"
mkdir "$BASE/$NAME/usr/share"
mkdir "$TARGET"
mkdir "$TARGET/storage"

# create service folder structure
mkdir "$BASE/$NAME/etc"
mkdir "$BASE/$NAME/etc/systemd"
mkdir "$BASE/$NAME/etc/systemd/system"

# create library folder structure
mkdir "$BASE/$NAME/usr/lib"
mkdir "$BASE/$NAME/usr/include"

# copy management application
cp -r "$BASE/static" "$TARGET"
cp -r "$BASE/templates" "$TARGET"
cp "$BASE/dist/static-app" "$TARGET/sdos-management"
cp "$BASE/README.md" "$TARGET"

# copy service file
cp "$BASE/sdos.service" "$BASE/$NAME/etc/systemd/system"

# copy c library
cp "$BASE/../sdos_c_lib/sdos.h" "$BASE/$NAME/usr/include"
cp "$BASE/../sdos_c_lib/cmake-build-debug/libsdos.so" "$BASE/$NAME/usr/lib"

# Write .deb file metadata
printf "Package: $PROG\n" >> "$BASE/$NAME/DEBIAN/control"
printf "Version: $VER\n" >> "$BASE/$NAME/DEBIAN/control"
printf "Architecture: all\n" >> "$BASE/$NAME/DEBIAN/control"
printf "Maintainer: Markus Lehr sdos.lehr@gmail.com\n" >> "$BASE/$NAME/DEBIAN/control"
printf "Depends: debconf\n" >> "$BASE/$NAME/DEBIAN/control"
printf "Installed-Size: 100\n" >> "$BASE/$NAME/DEBIAN/control"
printf "Homepage: https://github.com/sdos-lehr/sdos\n" >> "$BASE/$NAME/DEBIAN/control"
printf "Description: Management TCP server written in python for SDOS backend.\n" >> "$BASE/$NAME/DEBIAN/control"

# Write .deb post install script
POSTINST="$BASE/$NAME/DEBIAN/postinst"
printf "systemctl daemon-reload\n" >> "$POSTINST"
printf "systemctl enable sdos.service\n" >> "$POSTINST"
printf "systemctl restart sdos.service\n" >> "$POSTINST"

# Write .deb templates file
TEMPLATES="$BASE/$NAME/DEBIAN/templates"
printf "Template: sdos/management-ip\n" >> "$TEMPLATES"
printf "Type: string\n" >> "$TEMPLATES"
printf "Default: localhost\n" >> "$TEMPLATES"
printf "Description: The IP address to be used for the SDOS management server.\n" >> "$TEMPLATES"
printf " Will be stored under /usr/share/sdos/management-ip.\n" >> "$TEMPLATES"

# Write .deb config file
CONFIG="$BASE/$NAME/DEBIAN/config"
printf "#!/bin/sh -e\n">> "$CONFIG"
printf "# Source debconf library.\n">> "$CONFIG"
printf ". /usr/share/debconf/confmodule\n">> "$CONFIG"
printf "mkdir /usr/share/sdos\n">> "$CONFIG"
printf "db_input medium sdos/management-ip || true\n">> "$CONFIG"
printf "db_go\n">> "$CONFIG"
printf "db_get sdos/management-ip\n">> "$CONFIG"
printf "echo -n \"\$RET\" > /usr/share/sdos/management-ip" >> "$CONFIG"

chown root:root -R "$BASE/$NAME"
chmod -R 0755 "$BASE/$NAME"
chmod 644 "$BASE/$NAME/etc/systemd/system"

dpkg -b "$BASE/$NAME"