# Makefile for cpuload linux test program
#
# Copyright David Haworth
# Advanced testsuite implementation by knilch
#
# This file is part of Dave's OS toolkit.
#
# Dave's OS toolkit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Dave's OS toolkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Dave's OS toolkit.  If not, see <http://www.gnu.org/licenses/>.

cpuload-linux:		cpuload.o cpuload-linux.o

.PHONY:	clean test
clean:
	-rm cpuload-linux *.o

test: cpuload-linux
	tests/test.sh
