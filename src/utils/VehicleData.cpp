// Static vehicle lists used by forms and demo data.

#include "utils/VehicleData.hpp"



const char* k_stageNames[] = {
    "PRODUCTION", "WAREHOUSE_INTAKE", "QUALITY_CHECK",
    "DEALER_DISPATCH", "CUSTOMER_SALE"
};
const int k_stageNameCount = 5;



static const char* k_models_perodua[]    = {"Myvi","Axia","Bezza","Aruz","Ativa","Alza"};
static const char* k_models_proton[]     = {"Saga","Persona","X50","X70","X90","Iriz","Exora","S70"};
static const char* k_models_toyota[]     = {"Vios","Camry","Yaris","Corolla Cross","Hilux","Innova","Rush","Fortuner","Veloz"};
static const char* k_models_honda[]      = {"City","Civic","HR-V","CR-V","Accord","BR-V","WR-V"};
static const char* k_models_nissan[]     = {"Almera","X-Trail","Navara","Serena","Kicks"};
static const char* k_models_mazda[]      = {"Mazda 2","Mazda 3","CX-3","CX-5","CX-8","CX-30","BT-50","MX-5"};
static const char* k_models_mitsubishi[] = {"Triton","Xpander","Outlander","ASX"};
static const char* k_models_suzuki[]     = {"Swift","Vitara","Jimny","S-Cross","Ertiga","XL7"};
static const char* k_models_hyundai[]    = {"Kona","Tucson","Santa Fe","Stargazer","Ioniq 5","Ioniq 6","Palisade"};
static const char* k_models_kia[]        = {"Seltos","Sportage","Carnival","Cerato","EV6","EV9"};
static const char* k_models_bmw[]        = {"3 Series","5 Series","X1","X3","X5","iX","i4","i5"};
static const char* k_models_merc[]       = {"A-Class","C-Class","E-Class","GLA","GLC","GLE","EQA","EQB","EQE"};
static const char* k_models_vw[]         = {"Golf","Tiguan","Arteon","ID.4","T-Cross","Passat"};
static const char* k_models_subaru[]     = {"XV","Forester","Outback","WRX","BRZ"};
static const char* k_models_isuzu[]      = {"D-Max","MU-X"};
static const char* k_models_ford[]       = {"Ranger","Everest","Territory"};
static const char* k_models_lexus[]      = {"NX","RX","ES","UX","IS","LC"};
static const char* k_models_volvo[]      = {"XC40","XC60","XC90","S60","S90","C40","EX30"};

const ModelList k_modelsByMfr[] = {
    {k_models_perodua,    6}, {k_models_proton,     8}, {k_models_toyota,    9},
    {k_models_honda,      7}, {k_models_nissan,     5}, {k_models_mazda,     8},
    {k_models_mitsubishi, 4}, {k_models_suzuki,     6}, {k_models_hyundai,   7},
    {k_models_kia,        6}, {k_models_bmw,        8}, {k_models_merc,      9},
    {k_models_vw,         6}, {k_models_subaru,     5}, {k_models_isuzu,     2},
    {k_models_ford,       3}, {k_models_lexus,      6}, {k_models_volvo,     7},
};



const char* k_colors[] = {
    "White","Black","Silver","Grey","Red","Blue",
    "Brown","Green","Yellow","Orange","Beige","Maroon"
};
const int k_colorCount = 12;

const char* k_transportModes[] = {
    "Truck","Trailer","Car Carrier","Self-Drive"
};
const int k_transportModeCount = 4;
