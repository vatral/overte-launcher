# README

This is the Overte launcher, used for launching the Overte interface.

## Purpose

This project has two purposes.

1. Be a launcher (obviously)
2. Be a convenient testing platform for a few things.

## What we're testing

* CMake 4
* CPack packaging
* Windows code signing
* GitHub actions
* Qt testing framework

The main project is just a bit large and slow and complex to build, so sometimes
it's nice to have something smaller to experiment with.

This usage is likely temporary until we work things out.


## Building

Building should be easy. We're using CMake, Qt, and Wix on Windows (latest version).

Signing is done with Azure Artifact Signing (at least currently)

