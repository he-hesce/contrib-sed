/***************************************************************************
 *   Copyright (C) by ETHZ/SED                                             *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Affero General Public License as published*
 * by the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Affero General Public License for more details.                     *
 *                                                                         *
 * Authors of the Software: Michael Fischer and Yannik Behr                *
 ***************************************************************************/

// -*- C++ -*-
#ifndef SedVsxMapping_h__
#define SedVsxMapping_h__

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <limits>

#include <seiscomp/logging/log.h>
#include <seiscomp/core/strings.h>

namespace ch {
namespace sed {

/**
 \brief A container for Vs30 (or any other Vs) values used in estimating
 site amplifications.

 \see testG()
 */
class Vs30Mapping {
public:

	/**
	 \brief A 3-tuple representing latitude, longitude, and Vs30 value.
	 */
	class Tuple {
	public:
		float _lat;
		float _lon;
		float _vsx;
		Tuple(float lat, float lon, float vsx);
		std::string toString();
	};

	/**
	 \brief Interface definiton.
	 */
	class TupleHandler {
	public:
		virtual ~TupleHandler() {}
	protected:

		/**
		 Vs30 default value in case a list element is not available for
		 a specific coordinate.
		 */
		float _vsdefault;

		std::istream * _isP;
		std::filebuf * _fbP;

		virtual bool open(std::string filename);
		virtual bool read() = 0;
		virtual void close();

	public:

		/**
		 Read Vs30 values from the given file and populate the respective
		 datastructures. See read() method of the inheriting classes for
		 details.
		 */
		virtual bool load(std::string filename);

		//
		virtual float getVsDefault();
		virtual void setVsDefault(float val);

		/**
		 Return the Vs30 value for the given coordiante. If not overloaded
		 the method returns the Vs30 default value.
		 */
		virtual float getVs(double lat, double lon);
	};

	/**
	 \brief Handler for Vs30 grid files.
	 */
	class TupleHandlerGrid: public TupleHandler {
	private:
		std::vector<Tuple> _tuplelist;
		std::vector<size_t> _rowidx;

		virtual bool read();

		float getRow(double lat, double lon, size_t l, size_t r);
		float getCol(double lat, double lon, size_t l, size_t r);

	public:
		virtual float getVs(double lat, double lon);
	};

	/**
	 \brief Handler for Vs30 list files.
	 */
	class TupleHandlerList: public TupleHandler {
	private:
		std::vector<Tuple> _tuplelist;

		virtual bool read();

	public:
		virtual float getVs(double lat, double lon);
	};

private:

	//
	static TupleHandler * _tuplehandlerP;

	//
	static Vs30Mapping * _vsxmappingP;

public:

	/**
	 There is just one supported type at the moment.
	 */
	static int TYPE_VS30;

	/**
	 Sets the Vs30 default value. A negative value disables the Vs30
	 lookup process.
	 */
	void setVsDefault(int vstype, float val);
	float getVsDefault(int vstype);

	/**
	 Return the Vs30 value for the given coordinate. If the default value
	 is negative, the default value is always returned and no lookup
	 is performed.
	 */
	float getVs(int vstype, double lat, double lon);

	/**
	 Create a singleton instance with vs30 values.

	 Only one file type is supported:
	 grid format: predefined Vs30 mapping are available from
	 http://earthquake.usgs.gov/hazards/apps/vs30/predefined.php, 
	 e.g., California.xyz:
	 \code
	 # lon           lat     velocity
	 -124.996        42.0458 150
	 -124.987        42.0458 150
	 ...
	 \endcode
	 */
	static Vs30Mapping * createInstance(std::string filename);

	/**
	 Return pointer to the singleton instance.
	 */
	static Vs30Mapping * sharedInstance();

	//
	static Vs30Mapping * destroyInstance(Vs30Mapping * vsxmappingP);
};

}
}

#endif
