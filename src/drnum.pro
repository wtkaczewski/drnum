# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# +                                                                      +
# + This file is part of DrNUM.                                          +
# +                                                                      +
# + Copyright 2013 numrax GmbH, enGits GmbH                              +
# +                                                                      +
# + DrNUM is free software: you can redistribute it and/or modify        +
# + it under the terms of the GNU General Public License as published by +
# + the Free Software Foundation, either version 3 of the License, or    +
# + (at your option) any later version.                                  +
# +                                                                      +
# + DrNUM is distributed in the hope that it will be useful,             +
# + but WITHOUT ANY WARRANTY; without even the implied warranty of       +
# + MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        +
# + GNU General Public License for more details.                         +
# +                                                                      +
# + You should have received a copy of the GNU General Public License    +
# + along with DrNUM. If not, see <http://www.gnu.org/licenses/>.        +
# +                                                                      +
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
TEMPLATE  = subdirs
LANGUAGE  = C++
CONFIG   += ordered recursive
CONFIG   += debug_and_release

include (drnum.pri)

SUBDIRS  += drnumlib
SUBDIRS  += shmlib
SUBDIRS  += applications

drnumlib.file = drnumlib/drnumlib.pro
shmlib.file   = shmlib/shmlib.pro

applications.file    = applications/applications.pro
applications.depends = drnumlib
