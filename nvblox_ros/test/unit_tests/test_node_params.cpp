// SPDX-FileCopyrightText: NVIDIA CORPORATION & AFFILIATES
// Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <rclcpp/rclcpp.hpp>

#include "nvblox_ros/node_params.hpp"

namespace nvblox
{

template<typename T> void testParam(rclcpp::Node * node, Param<T> & param)
{
  T from_ros{};
  const bool is_set = node->get_parameter<T>(param.description().name, from_ros);
  EXPECT_TRUE(is_set);
  EXPECT_EQ(from_ros, param.get());
}

void testStringParam(rclcpp::Node * node, StringParam & param)
{
  std::string from_ros;
  const bool is_set = node->get_parameter<std::string>(param.description().name, from_ros);
  EXPECT_TRUE(is_set);
  EXPECT_EQ(from_ros, param.get());
}

// The lidar_<i>.* params default to the global lidar_* params, so an intrinsics entry
// without overrides holds the global values.
void expectIntrinsicsMatchGlobalParams(
  const NvbloxNodeParams & params, const LidarIntrinsicsParams & lidar)
{
  EXPECT_EQ(lidar.width, params.lidar_width.get());
  EXPECT_EQ(lidar.height, params.lidar_height.get());
  EXPECT_EQ(lidar.vertical_fov_rad, params.lidar_vertical_fov_rad.get());
  EXPECT_EQ(lidar.min_valid_range_m, params.lidar_min_valid_range_m.get());
  EXPECT_EQ(
    lidar.use_non_equal_vertical_fov, params.use_non_equal_vertical_fov_lidar_params.get());
  EXPECT_EQ(
    lidar.min_angle_below_zero_elevation_rad, params.min_angle_below_zero_elevation_rad.get());
  EXPECT_EQ(
    lidar.max_angle_above_zero_elevation_rad, params.max_angle_above_zero_elevation_rad.get());
}

void testParamSize(const int expected_param_size, const int actual_param_size)
{
  if (actual_param_size != expected_param_size) {
    std::cout << "Size of parameter struct has changed. Add tests for any new parameters and "
      "update expected_param_size to "
              << actual_param_size << std::endl;
    EXPECT_TRUE(false);
  }
}

TEST(BaseNodeParams, initialize) {
  constexpr size_t kExpectedParamSize = 480;
  testParamSize(kExpectedParamSize, sizeof(BaseNodeParams));

  auto node = std::make_shared<rclcpp::Node>("node", rclcpp::NodeOptions());

  BaseNodeParams params;
  initializeBaseNodeParams(node.get(), &params);

  testStringParam(node.get(), params.global_frame);
  testParam<float>(node.get(), params.voxel_size);
  testParam<float>(node.get(), params.max_back_projection_distance);
  testParam<float>(node.get(), params.layer_visualization_min_tsdf_weight);
  testParam<float>(node.get(), params.layer_visualization_exclusion_height_m);
  testParam<float>(node.get(), params.layer_visualization_exclusion_radius_m);
  testParam<float>(node.get(), params.layer_streamer_bandwidth_limit_mbps);

  testStringParam(node.get(), params.esdf_mode_str);
  EXPECT_EQ(params.esdf_mode, EsdfMode::k2D);
  testStringParam(node.get(), params.mapping_type_str);
  EXPECT_EQ(params.mapping_type, MappingType::kStaticTsdf);
}

TEST(NvbloxNodeParams, initialize) {
  constexpr size_t kExpectedParamSize = 2184;
  testParamSize(kExpectedParamSize, sizeof(NvbloxNodeParams));

  auto node = std::make_shared<rclcpp::Node>("node", rclcpp::NodeOptions());

  NvbloxNodeParams params;
  initializeNvbloxNodeParams(node.get(), &params);

  EXPECT_EQ(params.cuda_stream_type, CudaStreamType::kBlocking);

  testStringParam(node.get(), params.pose_frame);
  testStringParam(node.get(), params.map_clearing_frame_id);
  testStringParam(node.get(), params.after_shutdown_map_save_path);
  testStringParam(node.get(), params.esdf_slice_bounds_visualization_attachment_frame_id);
  testStringParam(node.get(), params.workspace_height_bounds_visualization_attachment_frame_id);
  testStringParam(node.get(), params.ground_plane_visualization_attachment_frame_id);
  testParam(node.get(), params.ground_plane_visualization_side_length);
  testParam<bool>(node.get(), params.print_timings_to_console);
  testParam<bool>(node.get(), params.print_rates_to_console);
  testParam<bool>(node.get(), params.print_delays_to_console);
  testParam<bool>(node.get(), params.print_queue_drops_to_console);
  testParam<bool>(node.get(), params.use_non_equal_vertical_fov_lidar_params);
  testParam<bool>(node.get(), params.publish_esdf_distance_slice);
  testParam<bool>(node.get(), params.use_color);
  testParam<bool>(node.get(), params.use_lidar);
  testParam<bool>(node.get(), params.layer_visualization_undo_gamma_correction);
  testParam<bool>(node.get(), params.use_segmentation);

  testParam<int>(node.get(), params.maximum_input_queue_length);
  testParam<int>(node.get(), params.back_projection_subsampling);
  testParam<int>(node.get(), params.tick_period_ms);
  testParam<int>(node.get(), params.print_statistics_on_console_period_ms);
  testParam<int>(node.get(), params.num_cameras);
  testParam<int>(node.get(), params.num_lidars);
  testParam<int>(node.get(), params.lidar_width);
  testParam<int>(node.get(), params.lidar_height);

  testParam<float>(node.get(), params.lidar_vertical_fov_rad);
  testParam<float>(node.get(), params.lidar_min_valid_range_m);
  testParam<float>(node.get(), params.min_angle_below_zero_elevation_rad);
  testParam<float>(node.get(), params.max_angle_above_zero_elevation_rad);
  testParam<bool>(node.get(), params.use_lidar_motion_compensation);
  testParam<bool>(node.get(), params.pointcloud2_timestamps_are_relative);
  testParam<float>(node.get(), params.esdf_slice_bounds_visualization_side_length);
  testParam<float>(node.get(), params.workspace_height_bounds_visualization_side_length);
  testParam<float>(node.get(), params.integrate_depth_rate_hz);
  testParam<float>(node.get(), params.integrate_color_rate_hz);
  testParam<float>(node.get(), params.integrate_lidar_rate_hz);
  testParam<float>(node.get(), params.update_mesh_rate_hz);
  testParam<float>(node.get(), params.update_esdf_rate_hz);
  testParam<float>(node.get(), params.publish_layer_rate_hz);
  testParam<float>(node.get(), params.publish_debug_vis_rate_hz);
  testParam<float>(node.get(), params.decay_tsdf_rate_hz);
  testParam<float>(node.get(), params.decay_dynamic_occupancy_rate_hz);
  testParam<float>(node.get(), params.clear_map_outside_radius_rate_hz);
  testParam<float>(node.get(), params.esdf_and_gradients_unobserved_value);
  testParam<float>(node.get(), params.map_clearing_radius_m);

  // Without overrides the single lidar mirrors the global lidar params.
  ASSERT_EQ(static_cast<int>(params.lidar_intrinsics.size()), params.num_lidars.get());
  expectIntrinsicsMatchGlobalParams(params, params.lidar_intrinsics.front());
}

TEST(NvbloxNodeParams, perLidarIntrinsics) {
  constexpr int kNumLidars = 2;
  constexpr int kLidarOneWidth = 64;
  constexpr float kLidarOneVerticalFovRad = 0.5F;

  rclcpp::NodeOptions options;
  options.parameter_overrides(
    {
      rclcpp::Parameter("num_lidars", kNumLidars),
      rclcpp::Parameter("lidar_1.width", kLidarOneWidth),
      rclcpp::Parameter("lidar_1.vertical_fov_rad", kLidarOneVerticalFovRad),
  });
  auto node = std::make_shared<rclcpp::Node>("node", options);

  NvbloxNodeParams params;
  initializeNvbloxNodeParams(node.get(), &params);

  ASSERT_EQ(params.num_lidars.get(), kNumLidars);
  ASSERT_EQ(static_cast<int>(params.lidar_intrinsics.size()), kNumLidars);

  // Lidar 0 is not overridden and falls back to the global lidar params.
  expectIntrinsicsMatchGlobalParams(params, params.lidar_intrinsics[0]);

  // Lidar 1 keeps the global defaults for everything that is not overridden.
  const LidarIntrinsicsParams & lidar_one = params.lidar_intrinsics[1];
  EXPECT_EQ(lidar_one.width, kLidarOneWidth);
  EXPECT_EQ(lidar_one.vertical_fov_rad, kLidarOneVerticalFovRad);
  EXPECT_EQ(lidar_one.height, params.lidar_height.get());
  EXPECT_EQ(lidar_one.min_valid_range_m, params.lidar_min_valid_range_m.get());
  EXPECT_EQ(
    lidar_one.use_non_equal_vertical_fov,
    params.use_non_equal_vertical_fov_lidar_params.get());
  EXPECT_EQ(
    lidar_one.min_angle_below_zero_elevation_rad,
    params.min_angle_below_zero_elevation_rad.get());
  EXPECT_EQ(
    lidar_one.max_angle_above_zero_elevation_rad,
    params.max_angle_above_zero_elevation_rad.get());
}

TEST(FuserNodeParams, initialize) {
  constexpr size_t kExpectedParamSize = 920;
  testParamSize(kExpectedParamSize, sizeof(FuserNodeParams));

  auto node = std::make_shared<rclcpp::Node>("node", rclcpp::NodeOptions());

  FuserNodeParams params;
  initializeFuserNodeParams(node.get(), &params);

  testStringParam(node.get(), params.dataset_path);
  testParam<int>(node.get(), params.number_of_frames_to_integrate);
  testParam<bool>(node.get(), params.update_on_key);

  testStringParam(node.get(), params.dataset_type_str);
  EXPECT_EQ(params.dataset_type, RosDatasetType::kThreedMatch);

  testStringParam(node.get(), params.depth_topic);
  testStringParam(node.get(), params.depth_camera_info_topic);
  testStringParam(node.get(), params.color_topic);
  testStringParam(node.get(), params.color_camera_info_topic);
  testParam<float>(node.get(), params.tf_lead_time_s);
}

}  // namespace nvblox

int main(int argc, char ** argv)
{
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  testing::InitGoogleTest(&argc, argv);

  const char * rclcpp_argv[1] = {"node"};
  rclcpp::init(1, rclcpp_argv);

  return RUN_ALL_TESTS();
}
