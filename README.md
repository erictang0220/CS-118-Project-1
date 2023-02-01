# CS118 Project 1 Demo

This is the repo for winter 2023 cs118 project 1.

## Students
* Eric Tang 705579803
* Yolanda Chen 005323226

## Makefile

This provides a couple make targets for things.
By default (all target), it makes the `server` executables.

It provides a `clean` target, and `tarball` target to create the submission file as well.

You will need to modify the `Makefile` USERID to add your userid for the `.tar.gz` turn-in at the top of the file.

## Academic Integrity Note

You are encouraged to host your code in private repositories on [GitHub](https://github.com/), [GitLab](https://gitlab.com), or other places.  At the same time, you are PROHIBITED to make your code for the class project public during the class or any time after the class.  If you do so, you will be violating academic honestly policy that you have signed, as well as the student code of conduct and be subject to serious sanctions.

## Provided Files

`server.c` is the entry points for the server part of the project.

## Project Report
1. Run `make clean` to remove the old server executable
2. Run `make` to build the server executable
3. Run `./server`
4. Go to your local browser
5. Run URL `localhost:[port]/[filename]`
6. The file should be displayed on the browser
