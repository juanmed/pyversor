[![Build Status](https://travis-ci.org/tingelst/pyversor.svg?branch=master)](https://travis-ci.org/tingelst/pyversor)
# pyversor
Python bindings for the [Versor](https://github.com/wolftype/versor) geometric algebra library

## Installation

```
git clone git@github.com:juanmed/pyversor.git
cd pyversor
git submodule update --init --recursive
git submodule update --recursive
curl https://bootstrap.pypa.io/pip/2.7/get-pip.py --output get-pip.py
python2.7 get-pip.py
sudo apt install python2-dev
python2.7 -m pip install vispy==0.6.4 zmq
python2.7 -m pip install -e .
```

Open a terminal and test
```
$python2.7
import pyversor
```