/* Path.hpp
 * Copyright (C) 2021  Sven Jähnichen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT PATH WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BUTILITIES_PATH_HPP_
#define BUTILITIES_PATH_HPP_

#include <string>

#ifdef _WIN32
#define BUTILITIES_PATH_SLASH "\\"
#else
#define BUTILITIES_PATH_SLASH "/"
#endif

namespace BUtilities
{

class Path
{
public:
        Path () : dir_(), file_(), ext_() {}
        Path (const std::string& path) {split (path);}

        Path& operator= (const std::string& path)
        {
                split (path);
                return *this;
        }

        operator std::string() const {return (dir().empty() || (dir() == BUTILITIES_PATH_SLASH) ? dir() : dir() + BUTILITIES_PATH_SLASH) + filename();}

        std::string dir() const {return dir_;}
        std::string filename() const {return file_ + (ext_.empty() ? "" : "." + ext_);}
        std::string ext() const {return ext_;}

protected:
        std::string dir_;
        std::string file_;
        std::string ext_;

        void split (const std::string& path)
        {
                const size_t spos = path.find_last_of (BUTILITIES_PATH_SLASH);
                if (spos == std::string::npos) dir_ = "";
                else if (spos == 0) dir_ = BUTILITIES_PATH_SLASH;
                else dir_ = path.substr (0, spos);

                file_ = path.substr (spos + 1);

                if (file_ == "") ext_ = "";

                else if ((file_ == ".") || (file_ == ".."))
                {
                        dir_ = (dir_.empty() || (dir_ == BUTILITIES_PATH_SLASH) ? dir_ : dir_ + BUTILITIES_PATH_SLASH) + file_;
                        file_ = "";
                        ext_ = "";
                }

                else
                {
                        const size_t dpos = file_.find_last_of (".");
                        if ((dpos != std::string::npos) && (dpos != 0))
                        {
                                ext_ = file_.substr (dpos + 1);
                                file_ = file_.substr (0, dpos);
                        }

                        else ext_ = "";

                }
        }


};

}

#endif /* BUTILITIES_PATH_HPP_ */
