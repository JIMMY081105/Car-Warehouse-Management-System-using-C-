#include "utils/BranchMap.hpp"
#include "imgui.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

// ── Manufacturer & branch data (shared with main_gui.cpp via extern) ────

const char* k_manufacturers[] = {
    "Perodua", "Proton", "Toyota", "Honda", "Nissan", "Mazda",
    "Mitsubishi", "Suzuki", "Hyundai", "Kia", "BMW", "Mercedes-Benz",
    "Volkswagen", "Subaru", "Isuzu", "Ford", "Lexus", "Volvo"
};
const int k_manufacturerCount = sizeof(k_manufacturers) / sizeof(k_manufacturers[0]);

static const char* k_branches_perodua[] = {
    "Rawang, Selangor, Malaysia",
    "Alor Setar, Kedah, Malaysia",
    "Kuala Terengganu, Terengganu, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Kuching, Sarawak, Malaysia"
};
static const char* k_branches_proton[] = {
    "Tanjung Malim, Perak, Malaysia",
    "Batu Kawan, Penang, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Pasir Gudang, Johor, Malaysia",
    "Shah Alam, Selangor, Malaysia"
};
static const char* k_branches_toyota[] = {
    "Glenmarie, Selangor, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Taiping, Perak, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Toyota City, Aichi, Japan"
};
static const char* k_branches_honda[] = {
    "Pegoh, Melaka, Malaysia",
    "Kota Bharu, Kelantan, Malaysia",
    "Ipoh, Perak, Malaysia",
    "Tawau, Sabah, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Minato, Tokyo, Japan"
};
static const char* k_branches_nissan[] = {
    "Serendah, Selangor, Malaysia",
    "Langkawi, Kedah, Malaysia",
    "Kuala Terengganu, Terengganu, Malaysia",
    "Miri, Sarawak, Malaysia",
    "Pasir Gudang, Johor, Malaysia",
    "Yokohama, Kanagawa, Japan"
};
static const char* k_branches_mazda[] = {
    "Kulim, Kedah, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Sibu, Sarawak, Malaysia",
    "Sandakan, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Hiroshima, Hiroshima, Japan"
};
static const char* k_branches_mitsubishi[] = {
    "Pekan, Pahang, Malaysia",
    "Butterworth, Penang, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Okazaki, Aichi, Japan"
};
static const char* k_branches_suzuki[] = {
    "Shah Alam, Selangor, Malaysia",
    "Sungai Petani, Kedah, Malaysia",
    "Kuala Terengganu, Terengganu, Malaysia",
    "Sibu, Sarawak, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Hamamatsu, Shizuoka, Japan"
};
static const char* k_branches_hyundai[] = {
    "Gurun, Kedah, Malaysia",
    "Taiping, Perak, Malaysia",
    "Temerloh, Pahang, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Ulsan, Gyeongsang, South Korea"
};
static const char* k_branches_kia[] = {
    "Alor Setar, Kedah, Malaysia",
    "Ipoh, Perak, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Pasir Gudang, Johor, Malaysia",
    "Hwaseong, Gyeonggi, South Korea"
};
static const char* k_branches_bmw[] = {
    "Kulim, Kedah, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Pekan, Pahang, Malaysia",
    "Sandakan, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Munich, Bavaria, Germany"
};
static const char* k_branches_merc[] = {
    "Sitiawan, Perak, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Kota Bharu, Kelantan, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Stuttgart, Baden-Wurttemberg, Germany"
};
static const char* k_branches_vw[] = {
    "Pekan, Pahang, Malaysia",
    "Langkawi, Kedah, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Wolfsburg, Lower Saxony, Germany"
};
static const char* k_branches_subaru[] = {
    "Glenmarie, Selangor, Malaysia",
    "Kota Bharu, Kelantan, Malaysia",
    "Miri, Sarawak, Malaysia",
    "Tawau, Sabah, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Ota, Gunma, Japan"
};
static const char* k_branches_isuzu[] = {
    "Rawang, Selangor, Malaysia",
    "Sitiawan, Perak, Malaysia",
    "Alor Setar, Kedah, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Fujisawa, Kanagawa, Japan"
};
static const char* k_branches_ford[] = {
    "Sungai Petani, Kedah, Malaysia",
    "Ipoh, Perak, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Dearborn, Michigan, United States"
};
static const char* k_branches_lexus[] = {
    "Shah Alam, Selangor, Malaysia",
    "Taiping, Perak, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Tawau, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Tahara, Aichi, Japan"
};
static const char* k_branches_volvo[] = {
    "Glenmarie, Selangor, Malaysia",
    "Butterworth, Penang, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Sandakan, Sabah, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Gothenburg, Vastra Gotaland, Sweden"
};

const BranchList k_branchesByMfr[] = {
    {k_branches_perodua,    6}, {k_branches_proton,     6}, {k_branches_toyota,    6},
    {k_branches_honda,      6}, {k_branches_nissan,     6}, {k_branches_mazda,     6},
    {k_branches_mitsubishi, 6}, {k_branches_suzuki,     6}, {k_branches_hyundai,   6},
    {k_branches_kia,        6}, {k_branches_bmw,        6}, {k_branches_merc,      6},
    {k_branches_vw,         6}, {k_branches_subaru,     6}, {k_branches_isuzu,     6},
    {k_branches_ford,       6}, {k_branches_lexus,      6}, {k_branches_volvo,     6},
};

// ── Interactive world map with branch pins ──────────────────────────────

struct GeoPin { float lat; float lng; const char* label; const char* country; };

static const GeoPin k_allPins[] = {
    // Malaysia — scattered across all states
    { 3.3215f, 101.5770f, "Rawang",           "Malaysia"},
    { 6.1200f, 100.3700f, "Alor Setar",       "Malaysia"},
    { 5.3117f, 103.1324f, "K. Terengganu",    "Malaysia"},
    { 2.7300f, 101.9400f, "Seremban",         "Malaysia"},
    { 1.4927f, 103.7414f, "Johor Bahru",      "Malaysia"},
    { 1.5535f, 110.3593f, "Kuching",          "Malaysia"},
    { 3.6845f, 101.5216f, "Tanjung Malim",    "Malaysia"},
    { 5.2000f, 100.4375f, "Batu Kawan",       "Malaysia"},
    { 3.8077f, 103.3260f, "Kuantan",          "Malaysia"},
    { 5.9804f, 116.0735f, "Kota Kinabalu",    "Malaysia"},
    { 1.4700f, 103.9100f, "Pasir Gudang",     "Malaysia"},
    { 3.0733f, 101.5185f, "Shah Alam",        "Malaysia"},
    { 3.1000f, 101.5833f, "Glenmarie",        "Malaysia"},
    { 5.4141f, 100.3288f, "Georgetown",       "Malaysia"},
    { 4.8500f, 100.7300f, "Taiping",          "Malaysia"},
    { 2.3167f, 102.1500f, "Pegoh",            "Malaysia"},
    { 6.1200f, 102.2400f, "Kota Bharu",       "Malaysia"},
    { 4.5975f, 101.0901f, "Ipoh",             "Malaysia"},
    { 4.2400f, 117.8900f, "Tawau",            "Malaysia"},
    { 3.3667f, 101.6000f, "Serendah",         "Malaysia"},
    { 6.3500f,  99.8000f, "Langkawi",         "Malaysia"},
    { 4.4000f, 114.0000f, "Miri",             "Malaysia"},
    { 1.8500f, 102.9300f, "Batu Pahat",       "Malaysia"},
    { 5.3833f, 100.5667f, "Kulim",            "Malaysia"},
    { 2.3000f, 111.8300f, "Sibu",             "Malaysia"},
    { 3.4833f, 103.3833f, "Pekan",            "Malaysia"},
    { 5.8400f, 118.1200f, "Sandakan",         "Malaysia"},
    { 5.6500f, 100.4900f, "Sungai Petani",    "Malaysia"},
    { 5.8167f, 100.4667f, "Gurun",            "Malaysia"},
    { 3.4500f, 102.4200f, "Temerloh",         "Malaysia"},
    { 5.4000f, 100.3700f, "Butterworth",      "Malaysia"},
    { 4.2200f, 100.7000f, "Sitiawan",         "Malaysia"},
    // Japan
    {35.0853f, 137.1577f, "Toyota City",      "Japan"},
    {35.6581f, 139.7514f, "Minato, Tokyo",    "Japan"},
    {35.4437f, 139.6380f, "Yokohama",         "Japan"},
    {34.3853f, 132.4553f, "Hiroshima",        "Japan"},
    {34.9551f, 137.1629f, "Okazaki",          "Japan"},
    {34.7108f, 137.7261f, "Hamamatsu",        "Japan"},
    {36.2959f, 139.3784f, "Ota, Gunma",       "Japan"},
    {35.3390f, 139.4870f, "Fujisawa",         "Japan"},
    {34.6693f, 137.0334f, "Tahara",           "Japan"},
    // South Korea
    {35.5384f, 129.3114f, "Ulsan",            "South Korea"},
    {37.1997f, 126.8313f, "Hwaseong",         "South Korea"},
    // Germany
    {48.1351f,  11.5820f, "Munich",           "Germany"},
    {48.7758f,   9.1829f, "Stuttgart",        "Germany"},
    {52.4227f,  10.7865f, "Wolfsburg",        "Germany"},
    // USA
    {42.3223f, -83.1763f, "Dearborn",         "United States"},
    // Sweden
    {57.7089f,  11.9746f, "Gothenburg",       "Sweden"},
};
static const int k_allPinsCount = sizeof(k_allPins) / sizeof(k_allPins[0]);

static float g_globeRotLon  = 110.0f;   // center longitude (starts at Malaysia)
static float g_globeRotLat  = 5.0f;     // center latitude
static float g_globeRadius  = 0.30f;    // radius fraction of min(w,h)/2

// Simplified coastline outlines
static const float k_peninsular[][2] = {
    {6.5f,100.0f},{6.2f,100.2f},{5.8f,100.3f},{5.5f,100.2f},{5.3f,100.3f},
    {4.9f,100.4f},{4.2f,100.6f},{3.7f,101.0f},{3.1f,101.3f},{2.5f,101.7f},
    {1.8f,102.3f},{1.4f,103.5f},{1.3f,103.8f},{1.4f,104.0f},{1.6f,104.2f},
    {2.0f,103.4f},{2.5f,103.4f},{3.1f,103.4f},{3.8f,103.4f},{4.2f,103.4f},
    {4.9f,103.4f},{5.5f,103.1f},{5.9f,102.4f},{6.2f,102.1f},{6.5f,101.8f},
    {6.7f,101.2f},{6.5f,100.0f},
};
static const float k_borneo[][2] = {
    {1.0f,109.6f},{1.3f,110.0f},{1.5f,110.3f},{1.6f,110.9f},{2.1f,111.3f},
    {2.5f,111.5f},{2.8f,111.9f},{3.2f,113.0f},{4.0f,114.1f},{4.3f,114.6f},
    {4.9f,115.0f},{5.3f,115.2f},{5.7f,115.6f},{6.0f,116.1f},{6.3f,116.8f},
    {6.5f,117.5f},{6.8f,117.6f},{7.0f,117.5f},{7.1f,117.0f},{6.8f,116.7f},
    {6.4f,116.5f},{5.8f,118.0f},{5.5f,118.3f},{5.0f,118.6f},{4.5f,118.1f},
    {4.0f,117.6f},{3.5f,117.3f},{3.0f,116.1f},{2.5f,115.5f},{2.0f,115.0f},
    {1.5f,114.5f},{1.2f,113.5f},{1.0f,112.5f},{0.9f,111.5f},{0.8f,110.5f},
    {1.0f,109.6f},
};
static const float k_japan[][2] = {
    {31.0f,131.0f},{33.0f,131.5f},{33.5f,132.0f},{34.0f,132.5f},{34.3f,133.0f},
    {34.5f,134.0f},{34.7f,135.0f},{35.0f,135.5f},{35.5f,136.0f},{36.0f,136.5f},
    {36.5f,137.0f},{37.0f,137.0f},{37.5f,138.5f},{38.0f,139.0f},{39.0f,140.0f},
    {40.0f,140.0f},{41.0f,140.5f},{41.5f,141.0f},{41.0f,141.5f},{40.0f,140.5f},
    {39.0f,140.5f},{38.0f,140.0f},{37.0f,139.5f},{36.5f,140.0f},{36.0f,140.5f},
    {35.5f,140.5f},{35.0f,140.0f},{34.5f,137.5f},{34.0f,136.5f},{33.5f,135.5f},
    {33.0f,134.0f},{32.5f,132.5f},{31.5f,131.5f},{31.0f,131.0f},
};
static const float k_korea[][2] = {
    {34.5f,126.5f},{35.0f,126.0f},{35.5f,126.5f},{36.0f,126.5f},{36.5f,126.5f},
    {37.0f,126.5f},{37.5f,126.5f},{38.0f,126.7f},{38.5f,127.0f},{38.5f,128.0f},
    {38.0f,128.5f},{37.5f,129.0f},{37.0f,129.4f},{36.5f,129.5f},{36.0f,129.5f},
    {35.5f,129.3f},{35.0f,129.0f},{34.5f,128.5f},{34.5f,127.5f},{34.5f,126.5f},
};
static const float k_europe[][2] = {
    {36.0f,-10.0f},{37.0f,-8.0f},{39.0f,-9.0f},{42.0f,-9.0f},{43.5f,-8.0f},
    {43.0f,-1.0f},{46.0f,-1.0f},{48.5f,-5.0f},{49.0f,-1.0f},{51.0f,1.0f},
    {53.5f,0.0f},{55.0f,8.0f},{54.0f,10.0f},{55.5f,12.5f},{56.0f,13.0f},
    {57.5f,12.0f},{58.5f,11.5f},{59.0f,11.0f},{62.0f,5.0f},{65.0f,12.0f},
    {69.0f,16.0f},{70.0f,20.0f},{69.0f,28.0f},{65.0f,30.0f},{60.0f,30.0f},
    {55.0f,28.0f},{50.0f,24.0f},{48.0f,17.0f},{46.0f,16.0f},{45.0f,14.0f},
    {44.0f,12.0f},{42.0f,12.0f},{39.0f,16.0f},{38.0f,16.0f},{37.0f,15.5f},
    {36.5f,12.5f},{37.5f,10.0f},{37.0f,1.0f},{36.5f,-2.0f},{36.0f,-6.0f},
    {36.0f,-10.0f},
};
static const float k_usa[][2] = {
    {25.0f,-80.0f},{30.0f,-82.0f},{30.0f,-90.0f},{29.0f,-95.0f},{26.0f,-97.0f},
    {32.0f,-106.0f},{32.0f,-117.0f},{34.0f,-120.0f},{37.0f,-122.0f},{40.0f,-124.0f},
    {46.0f,-124.0f},{48.5f,-123.0f},{49.0f,-95.0f},{49.0f,-83.0f},{46.0f,-83.0f},
    {42.0f,-83.0f},{42.0f,-71.0f},{40.0f,-74.0f},{37.0f,-76.0f},{35.0f,-76.0f},
    {33.0f,-79.0f},{30.0f,-81.0f},{25.0f,-80.0f},
};

// Africa
static const float k_africa[][2] = {
    {37.0f,-1.0f},{36.5f,3.0f},{37.0f,10.0f},{33.0f,12.0f},{32.0f,15.0f},
    {31.0f,25.0f},{31.5f,32.0f},{29.0f,33.0f},{22.0f,36.5f},{15.0f,40.0f},
    {12.0f,43.0f},{11.5f,51.0f},{5.0f,49.0f},{2.0f,45.0f},{-1.0f,42.0f},
    {-5.0f,40.0f},{-10.0f,40.5f},{-15.0f,40.5f},{-25.0f,35.0f},{-34.0f,26.0f},
    {-34.5f,18.0f},{-31.0f,17.0f},{-22.0f,14.0f},{-17.0f,11.0f},{-12.0f,13.0f},
    {-5.0f,12.0f},{0.0f,9.5f},{4.0f,7.0f},{6.0f,3.0f},{5.0f,-2.0f},
    {4.5f,-7.5f},{7.5f,-13.0f},{10.0f,-15.0f},{15.0f,-17.0f},{20.0f,-17.0f},
    {25.0f,-15.0f},{28.0f,-13.0f},{33.0f,-8.0f},{35.5f,-5.5f},{37.0f,-1.0f},
};
// South America
static const float k_south_america[][2] = {
    {12.0f,-72.0f},{10.5f,-75.0f},{8.0f,-77.0f},{4.0f,-77.5f},{1.0f,-80.0f},
    {-4.0f,-81.0f},{-6.0f,-81.0f},{-14.0f,-76.0f},{-18.0f,-70.0f},{-24.0f,-70.5f},
    {-33.0f,-72.0f},{-46.0f,-75.0f},{-52.0f,-70.0f},{-55.0f,-68.0f},{-54.0f,-65.0f},
    {-48.0f,-65.5f},{-42.0f,-63.0f},{-38.0f,-57.0f},{-34.5f,-54.0f},{-29.0f,-49.0f},
    {-23.0f,-43.0f},{-13.0f,-38.5f},{-5.0f,-35.0f},{-2.0f,-44.0f},{0.5f,-50.0f},
    {6.0f,-57.0f},{8.5f,-60.0f},{10.5f,-67.0f},{12.0f,-72.0f},
};
// Australia
static const float k_australia[][2] = {
    {-12.0f,136.0f},{-12.5f,130.5f},{-14.5f,129.0f},{-18.0f,122.0f},
    {-22.0f,114.0f},{-26.0f,113.0f},{-30.0f,115.0f},{-35.0f,117.5f},
    {-35.5f,137.0f},{-37.0f,140.0f},{-39.0f,146.0f},{-37.5f,150.0f},
    {-33.5f,152.0f},{-28.0f,153.5f},{-24.0f,152.0f},{-19.0f,147.0f},
    {-14.5f,143.5f},{-11.0f,136.5f},{-12.0f,136.0f},
};
// India
static const float k_india[][2] = {
    {35.0f,74.0f},{30.0f,78.0f},{27.0f,88.0f},{22.0f,89.0f},{21.0f,87.0f},
    {15.0f,80.0f},{8.0f,77.5f},{8.5f,76.0f},{13.0f,74.5f},{20.0f,73.0f},
    {24.0f,68.5f},{25.5f,62.0f},{29.0f,65.0f},{33.0f,72.0f},{35.0f,74.0f},
};
// China / mainland East Asia
static const float k_china[][2] = {
    {40.0f,73.5f},{45.0f,82.0f},{50.0f,87.5f},{53.5f,109.0f},{50.0f,117.0f},
    {48.0f,120.0f},{48.0f,135.0f},{43.0f,130.0f},{39.0f,122.0f},{35.0f,119.5f},
    {31.0f,122.0f},{25.0f,120.0f},{22.0f,114.0f},{21.5f,110.0f},{22.0f,106.0f},
    {21.5f,99.0f},{27.0f,97.0f},{35.0f,79.0f},{40.0f,73.5f},
};
// SE Asia mainland (Thailand, Vietnam, Myanmar, Cambodia, Laos)
static const float k_se_asia[][2] = {
    {22.0f,101.0f},{20.0f,100.0f},{17.0f,97.5f},{10.0f,98.5f},{7.0f,99.5f},
    {1.3f,103.8f},{1.4f,104.0f},{8.0f,105.0f},{10.5f,107.0f},{12.0f,109.0f},
    {16.5f,108.0f},{21.0f,106.0f},{23.5f,104.0f},{22.0f,101.0f},
};
// Arabian Peninsula / Middle East
static const float k_arabia[][2] = {
    {30.0f,35.0f},{25.0f,37.0f},{16.0f,43.0f},{13.0f,48.0f},{22.0f,59.5f},
    {26.0f,56.0f},{26.5f,50.0f},{29.0f,48.0f},{33.5f,36.0f},{30.0f,35.0f},
};
// Greenland
static const float k_greenland[][2] = {
    {60.0f,-45.0f},{65.0f,-54.0f},{72.0f,-56.0f},{78.0f,-68.0f},{82.0f,-55.0f},
    {83.0f,-40.0f},{78.0f,-18.0f},{70.0f,-22.0f},{62.0f,-42.0f},{60.0f,-45.0f},
};
// UK
static const float k_uk[][2] = {
    {50.0f,-5.5f},{51.5f,1.0f},{53.0f,0.0f},{55.5f,-2.0f},{58.5f,-3.0f},
    {58.5f,-5.0f},{56.0f,-5.5f},{54.0f,-4.5f},{51.5f,-5.0f},{50.0f,-5.5f},
};
// Central America / Mexico
static const float k_mexico[][2] = {
    {32.5f,-117.0f},{24.0f,-110.0f},{19.0f,-105.0f},{15.5f,-92.0f},{18.0f,-88.0f},
    {21.5f,-87.0f},{26.0f,-97.0f},{29.0f,-95.0f},{30.0f,-90.0f},{32.5f,-117.0f},
};
// Sumatra
static const float k_sumatra[][2] = {
    {5.5f,95.0f},{3.5f,98.5f},{0.0f,100.5f},{-3.0f,103.5f},{-5.8f,105.5f},
    {-3.5f,105.5f},{1.0f,102.0f},{5.0f,97.5f},{5.5f,95.0f},
};
// Java
static const float k_java[][2] = {
    {-6.0f,105.5f},{-7.0f,109.0f},{-8.0f,114.0f},{-8.5f,114.5f},
    {-7.5f,111.0f},{-6.5f,107.0f},{-6.0f,105.5f},
};
// Philippines
static const float k_philippines[][2] = {
    {18.5f,120.5f},{14.5f,120.0f},{13.0f,121.5f},{9.0f,126.0f},{6.5f,125.0f},
    {9.5f,124.0f},{12.5f,124.0f},{14.5f,121.0f},{18.5f,120.5f},
};
// New Zealand North Island
static const float k_nz_n[][2] = {
    {-34.5f,173.0f},{-37.5f,176.0f},{-41.5f,175.0f},{-38.5f,174.0f},{-34.5f,173.0f},
};
// New Zealand South Island
static const float k_nz_s[][2] = {
    {-41.0f,173.5f},{-44.0f,170.0f},{-46.5f,168.5f},{-44.0f,172.5f},{-41.0f,174.5f},{-41.0f,173.5f},
};
// Iceland
static const float k_iceland[][2] = {
    {63.5f,-20.0f},{64.0f,-14.0f},{66.0f,-14.0f},{66.5f,-18.0f},{65.5f,-24.0f},{63.5f,-20.0f},
};
// Madagascar
static const float k_madagascar[][2] = {
    {-12.0f,49.0f},{-15.5f,50.5f},{-22.0f,48.0f},{-25.5f,45.0f},{-23.0f,44.0f},
    {-16.0f,46.0f},{-12.0f,49.0f},
};
// Sri Lanka
static const float k_sri_lanka[][2] = {
    {9.5f,80.0f},{7.5f,81.5f},{6.0f,80.5f},{7.0f,79.5f},{9.5f,80.0f},
};
// Taiwan
static const float k_taiwan[][2] = {
    {25.0f,121.0f},{23.5f,121.5f},{22.0f,120.5f},{23.0f,120.0f},{25.0f,121.0f},
};
// Papua New Guinea
static const float k_png[][2] = {
    {-2.5f,141.0f},{-5.0f,141.5f},{-8.0f,143.0f},{-10.0f,147.0f},{-8.0f,148.5f},
    {-6.0f,147.5f},{-5.5f,150.0f},{-4.0f,152.5f},{-2.5f,150.0f},{-1.0f,141.0f},{-2.5f,141.0f},
};

#define OUTLINE_ENTRY(arr) {arr, sizeof(arr)/sizeof(arr[0])}
struct Outline { const float (*pts)[2]; int count; };
static const Outline k_outlines[] = {
    OUTLINE_ENTRY(k_peninsular), OUTLINE_ENTRY(k_borneo),
    OUTLINE_ENTRY(k_japan),      OUTLINE_ENTRY(k_korea),
    OUTLINE_ENTRY(k_europe),     OUTLINE_ENTRY(k_usa),
    OUTLINE_ENTRY(k_africa),     OUTLINE_ENTRY(k_south_america),
    OUTLINE_ENTRY(k_australia),  OUTLINE_ENTRY(k_india),
    OUTLINE_ENTRY(k_china),      OUTLINE_ENTRY(k_se_asia),
    OUTLINE_ENTRY(k_arabia),     OUTLINE_ENTRY(k_greenland),
    OUTLINE_ENTRY(k_uk),         OUTLINE_ENTRY(k_mexico),
    OUTLINE_ENTRY(k_sumatra),    OUTLINE_ENTRY(k_java),
    OUTLINE_ENTRY(k_philippines),OUTLINE_ENTRY(k_nz_n),
    OUTLINE_ENTRY(k_nz_s),      OUTLINE_ENTRY(k_iceland),
    OUTLINE_ENTRY(k_madagascar), OUTLINE_ENTRY(k_sri_lanka),
    OUTLINE_ENTRY(k_taiwan),     OUTLINE_ENTRY(k_png),
};
static const int k_outlineCount = sizeof(k_outlines)/sizeof(k_outlines[0]);

// Country center labels for hover
struct CountryCenter { float lat; float lon; const char* name; };
static const CountryCenter k_countries[] = {
    // Asia
    { 4.5f,  101.5f, "Malaysia"},       { 2.5f,  111.0f, "Malaysia"},
    {35.5f, 138.0f, "Japan"},
    {36.0f,  128.0f, "South Korea"},    {35.0f,  105.0f, "China"},
    {21.0f,   78.0f, "India"},          {15.5f,  101.0f, "Thailand"},
    {16.0f,  108.0f, "Vietnam"},        {12.5f,  105.0f, "Cambodia"},
    {19.0f,   97.0f, "Myanmar"},        { 1.3f,  103.8f, "Singapore"},
    {-2.0f,  118.0f, "Indonesia"},      {13.0f,  122.0f, "Philippines"},
    {23.7f,   90.4f, "Bangladesh"},     {28.4f,   84.1f, "Nepal"},
    {33.0f,   65.0f, "Afghanistan"},    {30.0f,   70.0f, "Pakistan"},
    { 7.0f,   80.0f, "Sri Lanka"},      {23.5f,  121.0f, "Taiwan"},
    {47.0f,  105.0f, "Mongolia"},       {37.0f,  127.5f, "North Korea"},
    {20.0f,  105.0f, "Laos"},
    // Middle East
    {24.0f,   45.0f, "Saudi Arabia"},   {32.0f,   54.0f, "Iran"},
    {33.0f,   44.0f, "Iraq"},           {26.0f,   50.5f, "Bahrain"},
    {25.3f,   51.2f, "Qatar"},          {24.0f,   54.0f, "UAE"},
    {21.0f,   57.0f, "Oman"},           {15.5f,   48.0f, "Yemen"},
    {35.0f,   38.0f, "Syria"},          {34.0f,   36.0f, "Lebanon"},
    {31.5f,   35.0f, "Israel"},         {31.0f,   36.0f, "Jordan"},
    {39.0f,   35.0f, "Turkey"},         {41.0f,   45.0f, "Georgia"},
    // Europe
    {48.8f,    2.3f, "France"},         {51.0f,   10.0f, "Germany"},
    {42.5f,   12.5f, "Italy"},          {40.0f,   -4.0f, "Spain"},
    {39.5f,   -8.0f, "Portugal"},       {54.0f,   -2.0f, "United Kingdom"},
    {53.0f,   -8.0f, "Ireland"},        {52.0f,    5.0f, "Netherlands"},
    {50.5f,    4.0f, "Belgium"},        {47.0f,    8.0f, "Switzerland"},
    {47.5f,   14.0f, "Austria"},        {49.8f,   15.5f, "Czech Republic"},
    {52.0f,   20.0f, "Poland"},         {47.5f,   19.0f, "Hungary"},
    {44.0f,   21.0f, "Serbia"},         {42.7f,   25.5f, "Bulgaria"},
    {44.5f,   26.0f, "Romania"},        {39.0f,   22.0f, "Greece"},
    {56.0f,   10.0f, "Denmark"},        {62.0f,   15.0f, "Sweden"},
    {64.0f,   26.0f, "Finland"},        {62.0f,   10.0f, "Norway"},
    {65.0f,  -18.0f, "Iceland"},        {46.0f,   25.0f, "Romania"},
    {48.7f,   19.7f, "Slovakia"},       {42.0f,   20.0f, "Kosovo"},
    {56.8f,   24.0f, "Latvia"},         {55.0f,   24.0f, "Lithuania"},
    {59.0f,   25.0f, "Estonia"},        {50.0f,   32.0f, "Ukraine"},
    // Africa
    { 0.0f,   38.0f, "Kenya"},          {-6.3f,   35.0f, "Tanzania"},
    {-1.3f,   36.8f, "Uganda"},         { 9.0f,    8.0f, "Nigeria"},
    {30.0f,    3.0f, "Algeria"},         {32.0f,   -5.0f, "Morocco"},
    {28.0f,   30.0f, "Egypt"},          {15.5f,   32.5f, "Sudan"},
    { 9.0f,   42.0f, "Ethiopia"},       {-14.0f,  34.0f, "Malawi"},
    {-29.0f,  24.0f, "South Africa"},   {-15.5f,  28.3f, "Zambia"},
    {-20.0f,  30.0f, "Zimbabwe"},       { 6.5f,   -1.5f, "Ghana"},
    {14.5f,  -14.5f, "Senegal"},        {-19.0f,  46.0f, "Madagascar"},
    {34.0f,    9.0f, "Tunisia"},        {-4.0f,   22.0f, "DR Congo"},
    { 7.5f,    1.5f, "Togo"},           {12.0f,  -15.0f, "Guinea"},
    // Americas
    {56.0f, -106.0f, "Canada"},         {39.0f,  -98.0f, "United States"},
    {23.0f, -102.0f, "Mexico"},         {-10.0f, -55.0f, "Brazil"},
    {-35.0f, -65.0f, "Argentina"},      {-33.5f, -71.0f, "Chile"},
    { 4.5f,  -74.0f, "Colombia"},       {-12.0f, -76.0f, "Peru"},
    {-17.0f, -65.0f, "Bolivia"},        {10.5f,  -66.9f, "Venezuela"},
    {-1.0f,  -78.0f, "Ecuador"},        {-33.0f, -56.0f, "Uruguay"},
    {-23.0f, -58.0f, "Paraguay"},       {18.5f,  -72.0f, "Haiti"},
    {19.0f,  -70.7f, "Dominican Republic"},
    {23.0f,  -82.0f, "Cuba"},           { 9.7f,  -84.0f, "Costa Rica"},
    {14.0f,  -87.0f, "Honduras"},       {13.2f,  -59.5f, "Barbados"},
    // Oceania
    {-25.0f, 134.0f, "Australia"},      {-42.0f, 172.0f, "New Zealand"},
    {-6.0f,  147.0f, "Papua New Guinea"},
    // Russia & Central Asia
    {60.0f,   100.0f, "Russia"},
    {41.0f,    65.0f, "Uzbekistan"},    {48.0f,    67.0f, "Kazakhstan"},
};
static const int k_countryCount = sizeof(k_countries)/sizeof(k_countries[0]);

void RenderBranchMap(float mapW, float mapH)
{
    const float PI = 3.14159265f;
    const float DEG2RAD = PI / 180.0f;

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImVec2 mapMin = origin;
    ImVec2 mapMax = ImVec2(origin.x + mapW, origin.y + mapH);
    ImDrawList* dl = ImGui::GetWindowDrawList();

    float cx = origin.x + mapW * 0.5f;
    float cy = origin.y + mapH * 0.5f;
    float radius = std::min(mapW, mapH) * g_globeRadius;

    // Dark space background
    dl->AddRectFilled(mapMin, mapMax, IM_COL32(2, 5, 15, 255));
    dl->PushClipRect(mapMin, mapMax, true);

    // Globe ocean sphere
    dl->AddCircleFilled(ImVec2(cx, cy), radius, IM_COL32(12, 25, 55, 255), 64);
    // Subtle highlight for 3D look
    dl->AddCircleFilled(ImVec2(cx - radius*0.18f, cy - radius*0.18f),
                        radius*0.55f, IM_COL32(20, 45, 90, 60), 48);

    // Atmosphere glow
    dl->AddCircle(ImVec2(cx, cy), radius + 2, IM_COL32(60, 130, 220, 35), 64, 3.0f);
    dl->AddCircle(ImVec2(cx, cy), radius + 6, IM_COL32(40, 90, 180, 15), 64, 2.0f);

    // Rotation parameters
    float cLatR = g_globeRotLat * DEG2RAD;
    float cosClat = std::cos(cLatR), sinClat = std::sin(cLatR);

    // Orthographic projection: geo (lat,lon) -> screen (x,y) + depth z
    auto GeoToGlobe = [&](float lat, float lon, float& outZ) -> ImVec2 {
        float latR = lat * DEG2RAD;
        float dLon = (lon - g_globeRotLon) * DEG2RAD;
        float cosLat = std::cos(latR), sinLat = std::sin(latR);
        float cosDlon = std::cos(dLon), sinDlon = std::sin(dLon);
        float x0 = cosLat * sinDlon;
        float y0 = cosClat * sinLat - sinClat * cosLat * cosDlon;
        float z0 = sinClat * sinLat + cosClat * cosLat * cosDlon;
        outZ = z0;
        return ImVec2(cx + x0 * radius, cy - y0 * radius);
    };

    // Grid lines (latitude circles)
    for (int lat = -60; lat <= 80; lat += 30) {
        ImVec2 prev; float prevZ = -1;
        for (int lon = -180; lon <= 180; lon += 3) {
            float z;
            ImVec2 p = GeoToGlobe((float)lat, (float)lon, z);
            if (prevZ > 0 && z > 0)
                dl->AddLine(prev, p, IM_COL32(30, 55, 80, 45), 0.5f);
            prev = p; prevZ = z;
        }
    }
    // Grid lines (longitude meridians)
    for (int lon = -180; lon < 180; lon += 30) {
        ImVec2 prev; float prevZ = -1;
        for (int lat = -90; lat <= 90; lat += 3) {
            float z;
            ImVec2 p = GeoToGlobe((float)lat, (float)lon, z);
            if (prevZ > 0 && z > 0)
                dl->AddLine(prev, p, IM_COL32(30, 55, 80, 45), 0.5f);
            prev = p; prevZ = z;
        }
    }

    // Land masses — filled polygons + outlines
    for (int oi = 0; oi < k_outlineCount; ++oi) {
        const Outline& ol = k_outlines[oi];
        // Collect visible projected points for fill
        std::vector<ImVec2> visPts;
        for (int j = 0; j < ol.count; ++j) {
            float z;
            ImVec2 p = GeoToGlobe(ol.pts[j][0], ol.pts[j][1], z);
            if (z > 0.0f) visPts.push_back(p);
        }
        if (visPts.size() >= 3)
            dl->AddConvexPolyFilled(visPts.data(), (int)visPts.size(),
                                    IM_COL32(25, 60, 35, 200));
        // Outline edges (only when both endpoints visible)
        for (int j = 0; j < ol.count - 1; ++j) {
            float z1, z2;
            ImVec2 p1 = GeoToGlobe(ol.pts[j][0], ol.pts[j][1], z1);
            ImVec2 p2 = GeoToGlobe(ol.pts[j+1][0], ol.pts[j+1][1], z2);
            if (z1 > 0 && z2 > 0)
                dl->AddLine(p1, p2, IM_COL32(50, 110, 70, 200), 1.0f);
        }
    }

    // Pins
    const char* hoveredLabel = nullptr;
    const char* hoveredCountry = nullptr;
    float hoveredLat = 0, hoveredLng = 0;
    char hoveredMfrs[512] = {};
    ImVec2 mousePos = ImGui::GetIO().MousePos;

    for (int i = 0; i < k_allPinsCount; ++i) {
        const GeoPin& pin = k_allPins[i];
        float z;
        ImVec2 sp = GeoToGlobe(pin.lat, pin.lng, z);
        if (z <= 0.05f) continue;  // behind the globe

        bool isMY = (strcmp(pin.country, "Malaysia") == 0);
        ImU32 pinCol  = isMY ? IM_COL32(0,200,120,255) : IM_COL32(255,100,60,255);
        ImU32 glowCol = isMY ? IM_COL32(0,200,120,50)  : IM_COL32(255,100,60,50);
        float pinR = 4.0f * (0.5f + 0.5f * z);  // perspective size
        dl->AddCircleFilled(sp, pinR + 3.0f, glowCol);
        dl->AddCircleFilled(sp, pinR, pinCol);
        dl->AddCircleFilled(sp, 1.5f, IM_COL32(255,255,255,220));

        // Label text (only when facing forward and globe is large enough)
        if (z > 0.3f && radius > 80)
            dl->AddText(ImVec2(sp.x + 7, sp.y - 6),
                        IM_COL32(200,210,220,(int)(180*z)), pin.label);

        float dx = mousePos.x - sp.x, dy = mousePos.y - sp.y;
        if (dx*dx + dy*dy < (pinR+6)*(pinR+6)) {
            hoveredLabel = pin.label;
            hoveredCountry = pin.country;
            hoveredLat = pin.lat; hoveredLng = pin.lng;
            hoveredMfrs[0] = '\0';
            for (int mi = 0; mi < k_manufacturerCount; ++mi) {
                const BranchList& bl = k_branchesByMfr[mi];
                for (int bi = 0; bi < bl.count; ++bi) {
                    if (strstr(bl.branches[bi], pin.label)) {
                        if (hoveredMfrs[0] != '\0') strcat(hoveredMfrs, ", ");
                        strcat(hoveredMfrs, k_manufacturers[mi]);
                        break;
                    }
                }
            }
        }
    }

    // Country name hover — unproject mouse to lat/lon on sphere
    const char* hoveredCountryName = nullptr;
    {
        float mx = (mousePos.x - cx) / radius;
        float my = -(mousePos.y - cy) / radius;
        float r2 = mx * mx + my * my;
        if (r2 < 1.0f) {
            float mz = std::sqrt(1.0f - r2);
            // Inverse rotation to get lat/lon
            float sinLat = sinClat * mz + cosClat * my;
            if (sinLat > 1.0f) sinLat = 1.0f;
            if (sinLat < -1.0f) sinLat = -1.0f;
            float lat = std::asin(sinLat) / DEG2RAD;
            float cosLatM = cosClat * mz - sinClat * my;
            float lon = g_globeRotLon + std::atan2(mx, cosLatM) / DEG2RAD;
            // Find nearest country center
            float bestDist = 1e9f;
            for (int ci = 0; ci < k_countryCount; ++ci) {
                float dlat = k_countries[ci].lat - lat;
                float dlon = k_countries[ci].lon - lon;
                if (dlon > 180.0f) dlon -= 360.0f;
                if (dlon < -180.0f) dlon += 360.0f;
                float d = dlat*dlat + dlon*dlon;
                if (d < bestDist) { bestDist = d; hoveredCountryName = k_countries[ci].name; }
            }
            // Only show if reasonably close (threshold depends on zoom)
            float thresh = 400.0f / (g_globeRadius * g_globeRadius + 1.0f);
            if (thresh < 20.0f) thresh = 20.0f;
            if (bestDist > thresh) hoveredCountryName = nullptr;
        }
    }

    dl->PopClipRect();

    // Mouse interaction
    ImGui::SetCursorScreenPos(mapMin);
    ImGui::InvisibleButton("##globe", ImVec2(mapW, mapH));
    bool hov = ImGui::IsItemHovered();

    if (hov && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 d = ImGui::GetIO().MouseDelta;
        float sensitivity = 0.195f / g_globeRadius;
        if (sensitivity < 0.0065f) sensitivity = 0.0065f;
        g_globeRotLon -= d.x * sensitivity;
        g_globeRotLat += d.y * sensitivity;
        if (g_globeRotLat >  85.0f) g_globeRotLat =  85.0f;
        if (g_globeRotLat < -85.0f) g_globeRotLat = -85.0f;
    }
    if (hov) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            g_globeRadius *= (wheel > 0) ? 1.16f : 0.87f;
            if (g_globeRadius < 0.20f) g_globeRadius = 0.20f;
            if (g_globeRadius > 150.0f) g_globeRadius = 150.0f;
        }
    }

    // Tooltip for pins
    if (hoveredLabel && hov) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1,1,1,1), "%s", hoveredLabel);
        ImGui::TextColored(ImVec4(0.6f,0.7f,0.8f,1), "%s  (%.2f, %.2f)",
                           hoveredCountry, hoveredLat, hoveredLng);
        if (hoveredMfrs[0] != '\0') {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.4f,0.9f,0.6f,1), "Brands: %s", hoveredMfrs);
        }
        ImGui::EndTooltip();
    }
    // Tooltip for country name (only when not hovering a pin)
    else if (hoveredCountryName && hov && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1,1,1,1), "%s", hoveredCountryName);
        ImGui::EndTooltip();
    }

    // --- Scale bar ---
    {
        // Earth radius ~ 3959 miles. 1 radian on sphere = 3959 miles.
        // On screen: 1 radian = radius pixels.
        // So miles_per_pixel = 3959 / radius.
        float milesPerPx = 3959.0f / radius;
        float targetBarPx = 80.0f;
        float rawMiles = targetBarPx * milesPerPx;
        // Pick nearest nice round number
        static const float nice[] = {1,2,5,10,20,50,100,200,500,1000,2000,5000,10000};
        float bestVal = nice[0];
        float bestDiff = 1e9f;
        for (int i = 0; i < 13; ++i) {
            float diff = std::abs(nice[i] - rawMiles);
            if (diff < bestDiff) { bestDiff = diff; bestVal = nice[i]; }
        }
        float barPx = bestVal / milesPerPx;
        if (barPx < 20.0f) barPx = 20.0f;
        if (barPx > 160.0f) barPx = 160.0f;

        char scaleBuf[64];
        if (bestVal >= 1.0f)
            snprintf(scaleBuf, sizeof(scaleBuf), "%.0f miles", bestVal);
        else
            snprintf(scaleBuf, sizeof(scaleBuf), "%.1f miles", bestVal);

        float sx = mapMax.x - barPx - 16.0f;
        float sy = mapMax.y - 18.0f;
        // Bar background
        dl->AddRectFilled(ImVec2(sx - 4, sy - 14), ImVec2(sx + barPx + 8, sy + 8),
                          IM_COL32(10, 20, 35, 200), 3.0f);
        // Bar line
        dl->AddLine(ImVec2(sx, sy), ImVec2(sx + barPx, sy), IM_COL32(200,210,220,200), 2.0f);
        // End caps
        dl->AddLine(ImVec2(sx, sy - 4), ImVec2(sx, sy + 4), IM_COL32(200,210,220,200), 1.5f);
        dl->AddLine(ImVec2(sx + barPx, sy - 4), ImVec2(sx + barPx, sy + 4),
                    IM_COL32(200,210,220,200), 1.5f);
        // Label
        dl->AddText(ImVec2(sx + 2, sy - 14), IM_COL32(200,210,220,220), scaleBuf);
    }

    // Legend (top-left)
    ImVec2 lp = ImVec2(mapMin.x + 6, mapMin.y + 6);
    dl->AddRectFilled(lp, ImVec2(lp.x + 195, lp.y + 28), IM_COL32(10,20,35,200), 4.0f);
    dl->AddCircleFilled(ImVec2(lp.x+12, lp.y+9), 4, IM_COL32(0,200,120,255));
    dl->AddText(ImVec2(lp.x+20, lp.y+2), IM_COL32(180,190,200,220), "Malaysia");
    dl->AddCircleFilled(ImVec2(lp.x+100, lp.y+9), 4, IM_COL32(255,100,60,255));
    dl->AddText(ImVec2(lp.x+108, lp.y+2), IM_COL32(180,190,200,220), "Origin HQ");
    dl->AddText(ImVec2(lp.x+6, lp.y+16), IM_COL32(120,130,140,180), "Drag to rotate, scroll to zoom");
}
