/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/Bynav.h"

std::ostream& operator<<(std::ostream& os, const Bynav& b)
{
  return os << "\n\n===================\n[BestGnssPos]"
            << "\nBestGnssPos solution status: " << static_cast<int>(b.best_gnss_pos_solution_status_)
            << "\nBestGnssPos position status: " << static_cast<int>(b.best_gnss_pos_position_status_) << "\nLatitude "
            << b.latitude_ << "\nLatitude std: " << b.latitude_std_ << "\nLongitude: " << b.longitude_
            << "\nLongitude std: " << b.longitude_std_ << "\nHeight: " << b.height_ << "\nHeight std: " << b.height_std_
            << "\nNumber of tracked satellites: " << static_cast<int>(b.best_gnss_pos_tracked_satellites_n_)
            << "\nNumber of satellites used in solution: " << static_cast<int>(b.best_gnss_pos_solution_satellites_n_)
            << "\n[Heading]"
            << "\nHeading solution status: " << static_cast<int>(b.heading_solution_status_)
            << "\nHeading position status: " << static_cast<int>(b.heading_position_status_)
            << "\nHeading solution source: " << static_cast<int>(b.heading_solution_source_)
            << "\nHeading baseline length: " << b.heading_baseline_length_ << "\nPitch: " << b.pitch_
            << "\nPitch std: " << b.pitch_std_ << "\nYaw(heading): " << b.yaw_ << "\nYaw(heading) std: " << b.yaw_std_;
}
