/*
**
* BEGIN_COPYRIGHT
*
* Copyright (C) 2008-2015 SciDB, Inc.
* All Rights Reserved.
*
* SciDB is free software: you can redistribute it and/or modify
* it under the terms of the AFFERO GNU General Public License as published by
* the Free Software Foundation.
*
* SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
* INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
* NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
* the AFFERO GNU General Public License for the complete license terms.
*
* You should have received a copy of the AFFERO GNU General Public License
* along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
*
* END_COPYRIGHT
*/

/*
 * PhysicalCrossBetween.cpp
 *
 *  Created on: May, 2016
 *  Author: Kriti Sen Sharma
 */

#include <query/PhysicalOperator.h>
#include <array/Metadata.h>
#include <array/Array.h>
#include "BetweenArray.h"
#include <util/SchemaUtils.h>

namespace scidb {

class PhysicalCrossBetween_: public  PhysicalOperator
{
public:
    PhysicalCrossBetween_(const std::string& logicalName, const std::string& physicalName, const Parameters& parameters, const ArrayDesc& schema):
         PhysicalOperator(logicalName, physicalName, parameters, schema)
    {
    }

    virtual PhysicalBoundaries getOutputBoundaries(const std::vector<PhysicalBoundaries> & inputBoundaries,
                                                  const std::vector< ArrayDesc> & inputSchemas) const //TODO: potential optimization (?) by comparing with corresponding code in `between` operator
    {
       return inputBoundaries[0];
    }

    Coordinates getWindowStart(Dimensions const& dims, const size_t offset) const
    {
        size_t nDims = dims.size();
        Coordinates result(nDims);
        for (size_t i = 0; i < nDims; i++)
        {
            Value const& coord = ((std::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[i + offset])->getExpression()->evaluate();
            if ( coord.isNull() || coord.get<int64_t>() < dims[i].getStartMin())
            {
                result[i] = dims[i].getStartMin();
            }
            else
            {
                result[i] = coord.get<int64_t>();
            }
        }
        return result;
    }

    Coordinates getWindowEnd(Dimensions const& dims, const size_t offset) const
    {
        size_t nDims = dims.size();
        Coordinates result(nDims);
        for (size_t i = 0; i < nDims; i++)
        {
            Value const& coord = ((std::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[i + offset + nDims])->getExpression()->evaluate();
            if (coord.isNull() || coord.getInt64() > dims[i].getEndMax())
            {
                result[i] = dims[i].getEndMax();
            }
            else
            {
                result[i] = coord.getInt64();
            }
        }
        return result;
    }

    /***
     * CrossBetween is a pipelined operator, hence it executes by returning an iterator-based array to the consumer
     * that overrides the chunkIterator method.
     */
    std::shared_ptr< Array> execute(std::vector< std::shared_ptr< Array> >& inputArrays,
                                      std::shared_ptr<Query> query)
    {
        assert(inputArrays.size() == 1);
        std::shared_ptr<Array> inputArray = ensureRandomAccess(inputArrays[0], query);

        // Some variables.
        SchemaUtils schemaUtilsInputArray(inputArray);
        Dimensions const& dims = schemaUtilsInputArray._dims;
        size_t nDims = dims.size();
        int64_t result;
        size_t nParams = _parameters.size();
        assert(nParams > 0 && nParams % (nDims*2) == 0); // assert that you have non-zero list of params,
        													// and that the number of parameters is a multiple of (nDims*2)

        // Scan all attributes of the rangesArray simultaneously, and fill in spatialRanges.
        // Set up a MultiConstIterators to process the array iterators simultaneously.
        SpatialRangesPtr spatialRangesPtr = std::make_shared<SpatialRanges>(nDims);

		    SpatialRange spatialRange(nDims);
        for (size_t i=0; i<nParams/(nDims*2); ++i) {
        	size_t offset = i*(nDims*2);
        	Coordinates lowPos = getWindowStart(dims, offset);
        	Coordinates highPos = getWindowEnd(dims, offset);
            if (isDominatedBy(lowPos, highPos)) {
                spatialRangesPtr->insert(SpatialRange(lowPos, highPos));
            }
        }
        spatialRangesPtr->buildIndex();

        // Return a CrossBetweenArray.
        return std::shared_ptr< Array>(std::make_shared<BetweenArray>(_schema, spatialRangesPtr, inputArray));
   }
};

REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalCrossBetween_, "cross_between_", "PhysicalCrossBetween_");

}  // namespace scidb
