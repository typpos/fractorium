# Linux Release

We will build a `.deb` binary and test it. Then we will build a source tarball,
upload to Launchpad PPA, let it autobuild, and copy the published `.deb` link
from there.

## Summary

### Building a `.deb` locally

Update `debian/changelog` with a new log message. Update version number. Use the time from `date -R`.

Build:

```
$ ./package-linux.sh --binary-only --unsigned
```

Type `s` for single binary, `Enter`. It displays a confirm message, `Enter`.

When the build finished, find the `.deb` in `$HOME/PPA/fractorium-VERSION`.

### Publishing to Launchpad PPA

Remember we are not building a `.deb` here, Launchpad will do that. We are just
uploading a signed source tarball.

Make sure `debian/changelog` was updated.

Build:

```
$ ./package-linux.sh
```

Type `s` for single binary, `Enter`. It displays a confirm message, `Enter`.

It asks for your GPG key passphrase to sign the source tarball.

Now upload the source tarball to Launchpad:

```
$ dput ppa:fractorium/ppa fractorium_1.0.0.0a-0ubuntu1_source.changes
```

An email will arrive to say if the package was accepted or rejected.

Wait for the autobuild to complete, and when the package is published, copy the
link to the `.deb` from "Package Details".

## Narrative

Starting with a fresh clone from my fork at bitbucket.

```
$ cd fractorium
```

First, testing locally on my machine.

`package-linux.sh` with no arguments builds a signed source for the Launchpad PPA.

Instead, now we want an unsigned binary `.deb`:

```
$ ./package-linux.sh --binary-only --unsigned
```

```
Error: Different version numbers were found. Please update the correct file,
the version numbers should agree up to the digits in:

0.9.9.6
1.0.0.0

./debian/changelog            : 0.9.9.6
./Source/Ember/EmberDefines.h : 1.0.0.0
```

So, I should bump the version number in `debian/changelog`. Let's get the time in the correct format:

```
$ date -R
Sat, 18 Jun 2016 13:12:15 +0100
```

Add a new log message at the top of `debian/changelog`, copying the last message
and changing the version number and time.

```
fractorium (1.0.0.0-0ubuntu1) xenial; urgency=low

  * release 1.0.0.0

 -- Gambhiro Bhikkhu <gambhiro.bhikkhu.85@gmail.com>  Sat, 18 Jun 2016 13:12:15 +0100
```

Now try again:

```
$ ./package-linux.sh --binary-only --unsigned
```

Question about type of build:

```
Type of package: single binary, indep binary, multiple binary, library, kernel module, kernel patch?
 [s/i/m/l/k/n]
```

Type `s` for single binary, `Enter`.

It displays a confirm message, `Enter`.

The build starts. It copied the files which are not ignored in `.gitignore` out
to `$HOME/PPA/fractorium-1.0.0.0/`, and building there.

The build finished, let's go and see:

```
$ cd $HOME/PPA/fractorium-1.0.0.0
$ ls -lh
total 7.7M
drwxrwxr-x 2 yume yume 4.0K Jun 18 13:31 build-area
drwxrwxr-x 8 yume yume 4.0K Jun 18 13:19 fractorium
-rw------- 1 yume yume  780 Jun 18 13:31 fractorium_1.0.0.0-0ubuntu1_amd64.changes
-rw------- 1 yume yume 3.5M Jun 18 13:31 fractorium_1.0.0.0-0ubuntu1_amd64.deb
-rw-rw-r-- 1 yume yume 2.1M Jun 18 13:18 fractorium_1.0.0.0.orig.tar.gz
-rw-rw-r-- 1 yume yume 2.1M Jun 18 13:18 fractorium-1.0.0.0.tar.gz
```

This `.deb` is ready to use.

(aside)

You could upload this `.deb` to the website if you don't want to bother with the
Launchpad PPA. The advantages of the PPA are

- testing the package and build procedure
- users will automatically get the updated version when they run their regular upgrades

ppl get used to always looking for the PPA of a project and installing from there:

```
sudo apt-add-repository ppa:fractorium/ppa
sudo apt-get update
sudo apt-get install fractorium
```

(aside end)

Back to installing the `.deb`:

```
sudo dpkg -i fractorium_1.0.0.0-0ubuntu1_amd64.deb
```

When I open fractorium with its menu icon, the About page has the updated
version number. I run a few random flames, drag some xforms around. All seems
well.

Now let's upload the source to the Launchpad PPA for the auto-build.

Back in the fractorium folder:

```
$ ./package-linux.sh
PPA work folder already exists: /home/yume/PPA/fractorium-1.0.0.0
Move this folder aside or remove it.
```

That's right, it expects to be able to create a clean folder with that name. I don't need the earlier files, so let's remove it:

```
$ rm -r /home/yume/PPA/fractorium-1.0.0.0
```

Try again:

```
$ ./package-linux.sh
```

Very good. Press `s` at the type of package question.

This is quick, it only creates a source tarball to upload.

Before finishing, it asks for my GPG key passphrase to sign the source tarball.

Now let's upload it to Launchpad:

```
$ cd /home/yume/PPA/fractorium-1.0.0.0/
$ dput ppa:fractorium/ppa fractorium_1.0.0.0-0ubuntu1_source.changes 
```

It verifies my signature and says `Uploading to ppa (via ftp to ppa.launchpad.net)`.

Upload finished, now I open my email and wait for a message from Launchpad. It
arrives, saying the package was accepted.

```
[~fractorium/ubuntu/ppa/xenial] fractorium 1.0.0.0-0ubuntu1 (Accepted)
```

I open https://launchpad.net/~fractorium/+archive/ubuntu/ppa from the email and check that the build started:

```
[BUILDING]	amd64 build of fractorium 1.0.0.0-0ubuntu1 in ubuntu xenial RELEASE
Build started 2 minutes ago on lgw01-09
```

Good, let's wait.

If the package had been rejected, or if the autobuild now fails, then I would be
fixing the build on my machine. When I want to try again, I would add a new
message to the top of `debian/changelog`, with a modified version number and
time. For these minor changes it is enough to append a letter to the version
number, such as `fractorium 1.0.0.0a`. Then create a tarball again with
`package-linux.sh`, and upload to Launchpad with `dput`.

If fact I just remembered I should update the project links and description in
`debian/control`. So I will do that now, bump the version number to
`fractorium 1.0.0.0a` and update the time to `date -R`.

Publish update:

```
$ ./package-linux.sh
$ cd ~/PPA/fractorium-1.0.0.0a
$ dput ppa:fractorium/ppa fractorium_1.0.0.0a-0ubuntu1_source.changes
```

Email says package accepted. Building has started. Finished:

```
Successfully built on lgw01-33

Started 23 minutes ago
Finished 1 minute ago (took 22 minutes, 6.6 seconds)
```

But also:

```
Binary packages awaiting publication:

fractorium-1.0.0.0a-0ubuntu1
```

It usually takes a bit more time until the `.deb` is published in the PPA repository.

Commit the changes:

```
$ git checkout -b linux-1.0.0.0a
$ git add -A .
$ git commit -m "linux 1.0.0.0a"
```

After a little while the package is published now, so I go to:

- https://launchpad.net/~fractorium/+archive/ubuntu/ppa
- Select "Package Details"
- Open the dropdown arrow at the package listing, see "Publishing Details"
- Under "Package Files", copy the link to the `.deb`

https://launchpad.net/~fractorium/+archive/ubuntu/ppa/+files/fractorium_1.0.0.0a-0ubuntu1_amd64.deb

Update the README.md link to this.

All done. Commit, push, send Pull Request.
