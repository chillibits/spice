// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#pragma once

#include <utility>

#include "SymbolTableEntry.h"

#include "../../lib/json/json.hpp"

enum CaptureMode { READ_ONLY, READ_WRITE };

class Capture {
public:
  // Constructors
  explicit Capture(SymbolTableEntry *entry) : capturedEntry(entry){};

  // Public methods
  [[nodiscard]] SymbolTableEntry *getEntry() const;
  void setCaptureMode(CaptureMode captureMode);
  [[nodiscard]] nlohmann::ordered_json toJSON() const;

private:
  // Members
  SymbolTableEntry *capturedEntry;
  CaptureMode mode = READ_ONLY;
};