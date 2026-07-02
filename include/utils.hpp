/*
 * GNU GENERAL PUBLIC LICENSE
 *
 * Copyright (C) 2021-2026
 * Created by Leonardo Parisi (leonardo.parisi[at]gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _H_CVL_UTILS_H_
#define _H_CVL_UTILS_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdarg>
#include <cctype>
#include <cmath>
#include <climits>
#include <cfloat>

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>

#include <unistd.h>    // getcwd, getuid
#include <pwd.h>       // getpwuid

#ifndef LINE_MAX
  #define LINE_MAX 2048
#endif

//*****************************************************************************
// namespace cvl
//*****************************************************************************
// Self-contained replacements for the few helpers that used to come from the
// external MPL library, so that CVL has no dependency on it.
namespace cvl {

  //***************************************************************************
  // namespace log - minimal logging used across CVL
  //***************************************************************************
  namespace log {

    //*************************************************************************
    // warning - print a message to stderr
    //*************************************************************************
    inline void warning(const char * format, ...) {

      va_list ap;
      va_start(ap, format);
      vfprintf(stderr, format, ap);
      va_end(ap);

      fflush(stderr);

    }

    //*************************************************************************
    // error - print a message to stderr and abort the program
    //*************************************************************************
    inline void error(const char * format, ...) {

      va_list ap;
      va_start(ap, format);
      vfprintf(stderr, format, ap);
      va_end(ap);

      fflush(stderr);

      exit(EXIT_FAILURE);

    }

  } // namespace log


  //***************************************************************************
  // namespace io - path and file helpers
  //***************************************************************************
  namespace io {

    namespace util {

      //***********************************************************************
      // resolveBasicPath - expand ~, relative and absolute paths
      //***********************************************************************
      inline void resolveBasicPath(const char * srcPath, char * dst) {

        // absolute path
        if(srcPath[0] == '/') {
          strcpy(dst, srcPath);
          return;
        }

        // home (~)
        if(srcPath[0] == '~') {

          struct passwd * pw = getpwuid(getuid());

          if(!pw) abort();

          strcpy(dst, pw->pw_dir);

          if(srcPath[1] == '/')
            strcat(dst, srcPath + 1);

          return;

        }

        // relative path
        if(getcwd(dst, PATH_MAX) == nullptr) abort();

        strcat(dst, "/");
        strcat(dst, srcPath);

      }

      //***********************************************************************
      // removeJunk
      //***********************************************************************
      inline void removeJunk(char * begin, char * end) {
        while(*end != 0) { *begin++ = *end++; }
        *begin = 0;
      }

      //***********************************************************************
      // manualPathFold - collapse /../, /./ and // when realpath fails
      //***********************************************************************
      inline char * manualPathFold(char * path) {

        char * s, * priorSlash;

        while((s = strstr(path, "/../")) != NULL) {
          *s = 0;
          if((priorSlash = strrchr(path, '/')) == NULL) { *s = '/'; break; }
          removeJunk(priorSlash, s + 3);
        }

        while((s = strstr(path, "/./")) != NULL) { removeJunk(s, s + 2); }
        while((s = strstr(path, "//"))  != NULL) { removeJunk(s, s + 1); }

        s = path + (strlen(path) - 1);

        if(s != path && *s == '/') { *s = 0; }

        return path;

      }

      //***********************************************************************
      // skipWhite - first non-whitespace char, or nullptr if none
      //***********************************************************************
      inline const char * skipWhite(const std::string & line) {

        for(size_t i=0; i<line.length(); ++i)
          if(!isspace((unsigned char)line[i])) return &line[i];

        return nullptr;

      }

    } // namespace util

    //*************************************************************************
    // expandPath
    //*************************************************************************
    inline void expandPath(const char * srcPath, char * destPath) {

      char buff[PATH_MAX+1];

      util::resolveBasicPath(srcPath, buff);

      if(realpath(buff, destPath) == nullptr)
        strcpy(destPath, util::manualPathFold(buff));

    }

    //*************************************************************************
    // expandPath - in place
    //*************************************************************************
    inline void expandPath(std::string & path) {

      char buff[PATH_MAX+1];

      expandPath(path.c_str(), buff);

      path = buff;

    }

    //*************************************************************************
    // basename
    //*************************************************************************
    inline std::string basename(const std::string & filename) {

      const char * p = strrchr(filename.c_str(), '/');

      if(p) return std::string(p + 1);
      else  return filename;

    }

    //*************************************************************************
    // subdirName - name of the directory containing the file
    //*************************************************************************
    inline std::string subdirName(const std::string & filePath) {

      if(filePath.empty() == false) {

        size_t toPos = filePath.find_last_of('/') - 1;

        if(toPos != std::string::npos) {

          size_t fromPos = filePath.find_last_of('/', toPos);

          if(fromPos != std::string::npos) {
            return filePath.substr(fromPos + 1, toPos - fromPos);
          }

        }

      }

      return "";

    }

    //*************************************************************************
    // isToSkip - true for empty, whitespace-only or comment (#) lines
    //*************************************************************************
    inline bool isToSkip(const std::string & line) {

      const char * ptr = util::skipWhite(line);

      // empty or whitespace-only line
      if(ptr == nullptr) return true;

      // comment line
      if(*ptr == '#') return true;

      return false;

    }

    //*************************************************************************
    // open
    //*************************************************************************
    inline FILE * open(const char * filepath, const char * mode) {

      char absolutePath[PATH_MAX];

      expandPath(filepath, absolutePath);

      FILE * file = fopen(absolutePath, mode);

      if(file == NULL) {
        cvl::log::error("cvl::io::open() error: cannot open the file '%s': %s\n", absolutePath, strerror(errno));
      }

      return file;

    }

    //*************************************************************************
    // open
    //*************************************************************************
    inline FILE * open(const std::string & filepath, const std::string & mode) {
      return open(filepath.c_str(), mode.c_str());
    }

    //*************************************************************************
    // close
    //*************************************************************************
    inline void close(FILE * file) {
      if(file != NULL) fclose(file);
    }

  } // namespace io




  //***************************************************************************
  // class loader - read selected columns from a text file
  //***************************************************************************
  class loader {

  public:

    //*************************************************************************
    // data - the values read from a single line
    //*************************************************************************
    class data {

    private:

      std::vector<std::string> arguments;

    public:

      std::string line;

      //***********************************************************************
      // add
      //***********************************************************************
      void add(std::string argument) { arguments.push_back(argument); }

      //***********************************************************************
      // size
      //***********************************************************************
      size_t size() const { return arguments.size(); }

      //***********************************************************************
      // get
      //***********************************************************************
      template <typename T>
      T get(size_t index) const {

        if(index >= arguments.size()) {
          cvl::log::error("cvl::loader::data::get() error: argument %zu does not exist\n", index);
        }

        std::stringstream iss(arguments[index]);

        T value;

        iss >> value;

        return value;

      }

    }; /* class data */

  private:

    //*************************************************************************
    // getColsToRead
    //*************************************************************************
    void getColsToRead(const std::string & colsFormat, std::vector<int> & columns) {

      std::stringstream iss(colsFormat);

      int value;

      while(iss >> value) {
        columns.push_back(value-1);
      }

    }

  public:

    //*************************************************************************
    // loader
    //*************************************************************************
    loader() {};

    //*************************************************************************
    // operator ()
    //*************************************************************************
    void operator () (const std::string & inputFile, const std::string & columnsToRead, std::function<void(const loader::data & arguments)> fillerFun = NULL) {

      if(fillerFun == NULL) fillerFun = std::bind(&loader::filler, this, std::placeholders::_1);

      FILE * input = io::open(inputFile, "r");

      std::vector<int> columns;

      getColsToRead(columnsToRead, columns);

      char line[LINE_MAX];

      char arg[LINE_MAX];

      while(fgets(line, LINE_MAX, input)) {

        if(io::isToSkip(line)) continue;

        int read     = 0;
        int colIndex = 0;

        loader::data arguments;

        arguments.line = line;

        int status = 0;
        int offset = 0;

        char * data = line;

        do {

          status = sscanf(data, "%s%n", arg, &offset);

          // no more tokens on this line: stop without touching stale data
          if(status != 1) break;

          data += offset;

          if(read != columns[colIndex]) { ++read; continue; }

          arguments.add(arg);

          if(read == columns.back()) { break; }

          ++colIndex;
          ++read;

        } while(status != EOF);

        // ogni colonna richiesta deve essere stata letta davvero
        if(arguments.size() != columns.size()) { cvl::log::error("cvl::loader error: wrong number of columns in '%s'\n", inputFile.c_str()); }

        fillerFun(arguments);

      }

      io::close(input);

    }

    //*************************************************************************
    // operator () - bind a member function of a container by reference
    //*************************************************************************
    template <typename T>
    inline void operator () (const std::string & inputFile, const std::string & columnsToRead, T & container, void(T::*fillerFun)(const loader::data & arguments)) {
      (*this)(inputFile, columnsToRead, std::bind(fillerFun, &container, std::placeholders::_1));
    }

    //*************************************************************************
    // operator () - bind a member function of a container by pointer
    //*************************************************************************
    template <typename T>
    inline void operator () (const std::string & inputFile, const std::string & columnsToRead, T * container, void(T::*fillerFun)(const loader::data & arguments)) {
      (*this)(inputFile, columnsToRead, std::bind(fillerFun, container, std::placeholders::_1));
    }

  protected:

    //*************************************************************************
    // filler
    //*************************************************************************
    virtual void filler(const loader::data & arguments) { };

  }; /* class loader */

  //*************************************************************************
  // data::get - specialization returning the raw string
  //*************************************************************************
  template <>
  inline std::string loader::data::get(size_t index) const {

    if(index >= arguments.size()) {
      cvl::log::error("cvl::loader::data::get() error: argument %zu does not exist\n", index);
    }

    return arguments[index];

  }


  //***************************************************************************
  // namespace utils - numeric / string / clustering helpers
  //***************************************************************************
  namespace utils {

    //*************************************************************************
    // median
    //*************************************************************************
    template <typename T>
    double median(const T & _set, double * stddev = NULL) {

      if(_set.size() == 0) {
        cvl::log::error("cvl::utils::median() error: the set is empty\n");
      }

      T set = _set;

      std::size_t halfSize = set.size() * 0.5;

      // partial selection (O(n)) is enough to locate the median element(s),
      // no need to fully sort the container
      std::nth_element(set.begin(), set.begin() + halfSize, set.end());

      double median = 0;

      if((set.size() % 2) != 0) {
        median = set[halfSize];
      } else {
        // for an even count the lower median is the max of the left partition
        double upper = set[halfSize];
        double lower = *std::max_element(set.begin(), set.begin() + halfSize);
        median = (lower + upper) * 0.5;
      }

      if(stddev != NULL) {

        if(set.size() < 2) {

          *stddev = 0;

        } else {

          double sum = 0;

          for(auto value=set.begin(); value!=set.end(); value++)
            sum += (*value - median) * (*value - median);

          *stddev = sqrt(sum / (double)(set.size()-1));

        }

      }

      return median;

    }

    //*************************************************************************
    // defaultDistanceOp
    //*************************************************************************
    constexpr auto defaultDistanceOp = [](auto const & a, auto const & b) { return cv::norm(a - b); };

    //*************************************************************************
    // medianFirstNNDistance - median of the first nearest-neighbor distances
    //*************************************************************************
    template <typename T, typename Op = decltype(defaultDistanceOp)>
    double medianFirstNNDistance(const T & data, Op op = defaultDistanceOp) {

      std::vector<float> minDist(data.size(), FLT_MAX);

      for(std::size_t i=0; i<data.size(); ++i) {

        for(std::size_t j=i+1; j<data.size(); ++j) {

          double dist = op(data[i], data[j]);

          if(dist < minDist[i]) minDist[i] = dist;
          if(dist < minDist[j]) minDist[j] = dist;

        }

      }

      return median(minDist);

    }

    //*************************************************************************
    // connectedComponents - cluster by distance threshold
    //*************************************************************************
    template <typename T, typename Op = decltype(defaultDistanceOp)>
    void connectedComponents(const T & data, double maxDist, std::vector<std::vector<std::size_t>> & clusters, int minClusterSize = 0, Op op = defaultDistanceOp) {

      clusters.clear();

      std::vector<bool> isTaken(data.size(), false);

      for(std::size_t i=0; i<data.size(); ++i) {

        if(!isTaken[i]) {

          isTaken[i] = true;

          std::vector<std::size_t> tmpCluster;

          tmpCluster.push_back(i);

          for(std::size_t j=0; j<tmpCluster.size(); ++j) {

            for(std::size_t k=0; k<data.size(); ++k) {

              if(!isTaken[k]) {

                double dist = op(data[tmpCluster[j]], data[k]);

                if(dist <= maxDist) {
                  isTaken[k] = true;
                  tmpCluster.push_back(k);
                }

              }

            }

          }

          if(tmpCluster.size() >= (std::size_t) minClusterSize)
            clusters.push_back(tmpCluster);

        }

      }

    }

    //*************************************************************************
    // connectedComponentsMedianFirstNN
    //   cluster using the median first nearest-neighbor distance as threshold
    //*************************************************************************
    template <typename T, typename Op = decltype(defaultDistanceOp)>
    void connectedComponentsMedianFirstNN(const T & data, std::vector<std::vector<std::size_t>> & clusters, double factor = 1.0, int minClusterSize = 0, Op op = defaultDistanceOp) {

      double NNDistance = medianFirstNNDistance(data, op);

      double maxDist = NNDistance * factor;

      connectedComponents(data, maxDist, clusters, minClusterSize, op);

    }

    //*************************************************************************
    // itoa - integer to ascii
    //*************************************************************************
    inline char * itoa(int value, char * result, int base = 10) {

      // check that the base is valid
      if(base < 2 || base > 36) {
        *result = '\0';
        return result;
      }

      char * ptr = result, * ptr1 = result, tmp_char;
      int tmp_value;

      do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
      } while(value);

      // apply negative sign
      if(tmp_value < 0) *ptr++ = '-';

      *ptr-- = '\0';

      while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
      }

      return result;

    }

    //*************************************************************************
    // decToHex - convert a decimal value (0-255) to a two-digit hex string
    //*************************************************************************
    inline std::string decToHex(int n) {

      if(n < 0 || n > 255) {
        cvl::log::error("cvl::utils::decToHex() error: value %d out of range [0,255]\n", n);
      }

      char hexaDeciNum[2];

      int i = 0;

      while(n != 0) {

        int temp = n % 16;

        if(temp < 10) {
          hexaDeciNum[i] = temp + 48;
          i++;
        } else {
          hexaDeciNum[i] = temp + 55;
          i++;
        }

        n = n / 16;

      }

      std::string hexCode = "";

      if(i == 2) {
        hexCode.push_back(hexaDeciNum[1]);
        hexCode.push_back(hexaDeciNum[0]);
      } else if(i == 1) {
        hexCode.push_back(48);
        hexCode.push_back(hexaDeciNum[0]);
      } else if(i == 0)
        hexCode = "00";

      return hexCode;

    }

  } // namespace utils

} /* namespace cvl */

#endif /* _H_CVL_UTILS_H_ */
