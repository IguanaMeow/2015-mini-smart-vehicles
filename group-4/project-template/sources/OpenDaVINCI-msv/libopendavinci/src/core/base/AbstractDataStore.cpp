/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "core/base/AbstractDataStore.h"
#include "core/base/Lock.h"

namespace core {
    namespace base {

        AbstractDataStore::AbstractDataStore() :
            m_condition() {}

        AbstractDataStore::~AbstractDataStore() {}

        void AbstractDataStore::waitForData() {
            Lock l(m_condition);
            if (isEmpty()) {
                m_condition.waitOnSignal();
            }
        }

        void AbstractDataStore::wait() {
          Lock l(m_condition);
          m_condition.waitOnSignal();
        }

        void AbstractDataStore::wakeAll() {
            Lock l(m_condition);
            m_condition.wakeAll();
        }


    }
} // core::base