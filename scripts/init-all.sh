#!/bin/sh
#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
# @TAG(DATA61_BSD)
#

pushd ./rumprun
git submodule init
git submodule update
pushd ./src-netbsd
git am ../src-netbsd.patches/*
popd
popd
pushd ./apps
echo "" > Kconfig
echo "include \${PWD}/rumprun/platform/sel4/rumprunlibs.mk" > Kbuild
# For each app in apps.txt symlink the app from userapps
# Then also populate the Kbuild and Kconfig files
# This populates code responsible for creating root filesystems for
# each app based on a path in the apps directory.txt. 
for i in `cat ../projects/rumprun-sel4-demoapps/apps.txt`; do
    ln -s ../projects/rumprun-sel4-demoapps/userapps/$i .
    dir="$i"
    echo "" >> Kconfig
    echo "menuconfig APP_${dir^^}" >> Kconfig
    echo "  bool \"$dir app\"" >> Kconfig
    echo "  default n" >> Kconfig
    echo "  help" >> Kconfig
    echo "      Rumprun Application: $dir" >> Kconfig

    echo "components-\$(CONFIG_APP_${dir^^}) += $dir" >> Kbuild
    echo "roottask-components-\$(CONFIG_APP_${dir^^}) += $dir" >> Kbuild
    echo "$dir-y = rumprun" >> Kbuild
    echo "" >> Kbuild
    echo "ifeq (\$(CONFIG_APP_${dir^^}), y)" >> Kbuild
    echo "dirpath:= \$(shell cat \$(PROJECT_BASE)/apps/$dir/directory.txt)" >> Kbuild
    echo "dirpathlen := \$(shell cat \$(PROJECT_BASE)/apps/$dir/directory.txt|wc -l)" >> Kbuild

    echo "ifneq (\$(dirpathlen), 1)" >> Kbuild
    echo "else" >> Kbuild
    echo "FULLDIRPATH := \$(PROJECT_BASE)/\$(dirpath)" >> Kbuild
    echo "endif" >> Kbuild
    echo "$dir-dir := \$(shell mkdir -p \$(SEL4_RROBJ)/rootfs/ && rsync -av \$(FULLDIRPATH) \\" >> Kbuild
    echo "	\$(PROJECT_BASE)/rumprun/lib/librumprunfs_base/rootfs/ \$(SEL4_RROBJ)/rootfs/ --delete | wc -l)" >> Kbuild
    echo "ifneq (\$($dir-dir), 4)" >> Kbuild
    echo "	export RUMPSTALE = 1" >> Kbuild
    echo "endif" >> Kbuild
    echo "endif" >> Kbuild
    echo "" >> Kbuild
    echo "$dir: \$($dir-y)" >> Kbuild
    echo "" >> Kbuild
done;
ln -s ../projects/rumprun-sel4-demoapps/userapps/rumpcommon.mk .
popd
