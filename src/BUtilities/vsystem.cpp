/* vsystem.cpp
 * Copyright (C) 2021  Sven Jähnichen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "vsystem.hpp"
#include <unistd.h>
#include <cstdio>

namespace BUtilities
{

[[noreturn]] static void vexec (char* argv[])
{
        execvp (argv[0], argv);
        perror ("execvp");
        _exit (1);
}

int vsystem (char* argv[])
{
        pid_t pid = vfork();
        if (pid != 0) return pid;
        vexec (argv);
        return 0;
}

}
