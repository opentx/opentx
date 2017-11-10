# Launchpad instructions

## General description

This directory contains files and scripts that help to prepare the files needed by the Launchpad.net server to build the Debian packages of OpentX Companion for Ubuntu.

Each supported release of Ubuntu has its own sub-directory, but most of the files reside in the common directory. When preparing a source for certain Ubuntu release always use its the directory name.

There are several step to release a new version of Companion on Launchpad:
 * update the Debian changelog
 * package the source
 * upload the source to Launchpad

The steps above should be repeated for every Ubuntu release. The example below will work with Ubuntu `trusty`

## Prerequisites

 * you must have launchpad.net account and be a member of the team
 * create and configure your GPG signing key

## Prepare your GPG key

Create GPG key using **name** and **email* that will later appear in the changelog. This will be the key
that will be used to sign the package before uploading it to Launchpad.
```
gpg --gen-key
```

Upload your GPG key (public part) to the Ubuntu key-server. First get the KEY ID (8 characters) of your key:
```
gpg --list-keys
```

Then upload it (replace <KEY ID> with your key ID):
```
gpg --send-keys --keyserver keyserver.ubuntu.com <KEY ID>
```

## Create a Launchpad account
 * Go to https://launchpad.net/ and sign-in or register.
 * Associate your GPG key with your launchpad account. You will need your keys fingerprint (<fingerprint looks like E448 37D6 EF6E F13C 30C0  415C E7A0 818F 4864 9459>), get it with:
```
gpg --fingerprint
```
 * Join OpenTX team on launchpad (one of admins will add you to the team).


## Step 1: update the Debian changelog

You can edit changelog manually, but it is simpler to use a script that creates a generic entry in the changelog. You can always edit it later if needed.

The script is `update-changelog.sh` and it takes 4 parameters:
 * email address in the form of `name <email@address>`, since it contains a space it must be enclosed in quotation marks. The email addres must be the same as the one in your GPG key!
 * Ubuntu release (directory name)
 * OpenTX version
 * OpenTX sub-version (Nxxx for nightly builds)

Example:
```
cd opentx/launchpad
./update-changelog.sh "projectkk2glider <projectkk2glider@gmail.com>" trusty 2.2.0 N363
```

If there is an error and you need to build another version of the same OpenTX release, then just add `.n` to the sub-version like so:
```
N363.1
N363.2
etc
```

The script will append new section to the existing file in this case `trusty/changelog`. Now you have a chance to edit the file manually if needed.

## Step 2: prepare files for upload (package the source)

First make sure you don't have any files that you don't want included in the opentx directory. Then use the script `prepare.sh` with the name of release as the first parameter:

```
cd opentx/launchpad
./prepare.sh trusty
```

The result of this script are three files which are placed outside the OpenTX source tree at the same level as `opentx` directory:
```
ls -1 ../../opentx-companion22_2.2.0~N363*
../../opentx-companion22_2.2.0~N363~trusty.dsc
../../opentx-companion22_2.2.0~N363~trusty_source.build
../../opentx-companion22_2.2.0~N363~trusty_source.changes
../../opentx-companion22_2.2.0~N363~trusty.tar.gz
```

You should now make sure that the just build source archive `opentx-companion22_2.2.0~N363~trusty.tar.gz` does not contain any files or directories that you don;t want to share with the world, because this is one of the files that will be uploaded to the Launchpad server. Currently clean OpenTX tree produces this file with approximate size of 16.8MB.

## Upload the source to the Launchpad

Last step it to upload the new release to the Launchpad server and wait for it to be built. Use `dput` command with two arguments:
 * name of the PPA `ppa:opentx-test/ppa` (your public key must be associated with that PPA)
 * name of the release file `<dsfsfggg>.changes`

Upload to nightly PPA:
```
dput ppa:opentx-test/ppa ../../opentx-companion22_2.2.0~N363~trusty_source.changes
```

Upload to release PPA:
```
dput ppa:opentx-test/rel ../../opentx-companion22_2.2.0~N363~trusty_source.changes
```

