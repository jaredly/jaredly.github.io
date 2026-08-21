const DATA = {
  neighborhoods: "data/neighborhoods.geojson",
  schools: "data/schools.json",
  privateSchools: "data/private_schools.json",
  metadata: "data/metadata.json",
  regions: {
    elementary: "data/school_regions_elementary.geojson",
    middle: "data/school_regions_middle.geojson",
    high: "data/school_regions_high.geojson",
  },
  demographics: "data/demographics_tracts.geojson",
  zips: "data/demographics_zips.geojson",
  daaBlockGroups: "data/daa_labs_block_groups.geojson",
  tornadoPolygons: "data/tornado_damage_polygons.geojson",
  tornadoPoints: "data/tornado_damage_points.geojson",
  tornadoPath: "data/tornado_path.geojson",
};

const HOME_VALUE_SCALE_MAX = 400000;
const SCHOOL_AGE_RACE_WAFFLE_UNIT = 20;
const PUBLIC_PRIVATE_ENROLLMENT_WAFFLE_UNIT = 20;

const schoolModes = [
  {
    id: "attendance",
    label: "Enrollment / Attendance / Capacity",
    description: "School symbols split enrolled students into 90%+ attendance and below-90% attendance groups, then add unused capacity seats so the footprint reflects both current enrollment and open seats.",
  },
  {
    id: "poverty",
    label: "Deep Poverty",
    description: "School symbols compare direct-certified students with other enrolled students. Direct certification indicates household participation in assistance programs used for free-meal eligibility.",
  },
  {
    id: "race",
    label: "Race",
    description: "School symbols split student enrollment into Black, White, and Other groups from NCES race/ethnicity counts. Other includes Hispanic students and all other reported race groups.",
  },
  {
    id: "enrollmentGrowth",
    label: "Enrollment Growth",
    description: "School symbols show the latest 10 years of annual enrollment as mini bar charts, scaled to each school's own maximum. Color shows the least-squares trend: growing, steady, or shrinking.",
  },
  {
    id: "assessmentPerformance",
    label: "MAP Performance",
    description: "School symbols show the selected MAP-tested content/grade performance mix: Below Basic, Basic, Proficient, Advanced, and Missing.",
  },
  {
    id: "assessmentIep",
    label: "MAP IEP Composition",
    description: "School symbols compare students with an IEP and students without an IEP within the selected MAP-tested content/grade denominator.",
  },
  {
    id: "pto",
    label: "PTO Activity",
    description: "School symbols show curated public evidence of school-level PTO activity, with confidence and source details available after selecting a school.",
  },
];

const schoolLevels = [
  { id: "all", label: "All" },
  { id: "none", label: "None" },
  { id: "elementary", label: "Elementary" },
  { id: "middle", label: "Middle" },
  { id: "high", label: "High" },
];

const schoolSectors = [
  { id: "slps", label: "SLPS" },
  { id: "charter", label: "Charter" },
];

const closurePlans = [
  { id: "none", label: "None" },
  { id: "model_a_status", label: "Plan A" },
  { id: "model_b_status", label: "Plan B" },
  { id: "model_c_status", label: "Plan C" },
];

const backgroundColorings = [
  { id: "none", label: "None" },
  { id: "race", label: "Race (2024 ACS)" },
  { id: "poverty", label: "Poverty Indicators (2024 ACS)" },
  { id: "tornado", label: "2025 Tornado Damage" },
  { id: "home", label: "Typical Home Value (ZHVI)" },
  { id: "schoolAgeTotal", label: "School-Age Children Total (2024 ACS)" },
  { id: "whiteSchoolAgeChildren", label: "White School-Age Children (2024 ACS)" },
  { id: "blackSchoolAgeChildren", label: "Black School-Age Children (2024 ACS)" },
  { id: "publicSchoolEnrollment", label: "Public-School Enrollment Ages 5-17 (2024 ACS)" },
  { id: "privateSchoolEnrollment", label: "Private-School Enrollment Ages 5-17 (2024 ACS)" },
  { id: "daaEnrollment2025Total", label: "SLPS Enrollment 2025-26 (DAA)" },
  { id: "daaEnrollmentChangeTotal", label: "SLPS Enrollment Change 2024-25 to 2025-26 (DAA)" },
  { id: "daaPopulation2020SchoolAge", label: "2020 School-Age Population (Census)" },
  { id: "daaEnrollmentPerSchoolAge", label: "SLPS 2025-26 Enrollment per 2020 School-Age Population" },
];

const backgroundWaffles = [
  { id: "none", label: "None" },
  { id: "schoolAgeRace", label: "School-Age Race (2024 ACS)" },
  { id: "publicPrivateSchoolEnrollment", label: "Public vs. Private Enrollment Ages 5-17 (2024 ACS)" },
];

const sidebarComparisons = [
  { id: "selection", label: "Selection / Summary" },
  { id: "closure", label: "Future Ready: Open vs Closed" },
  { id: "slpsCharter", label: "SLPS vs Charter" },
  { id: "slpsPrivate", label: "SLPS vs Private" },
  { id: "charterPrivate", label: "Charter vs Private" },
];

const backgroundColoringDescriptions = {
  none: "No background color layer is drawn.",
  race: "2024 ACS 5-year Census tract race estimates. The color scale shows the share of residents identified as non-Hispanic Black alone.",
  poverty: "2024 ACS 5-year Census tract poverty estimates. The color scale shows the share of residents below the federal poverty threshold.",
  tornado: "City tornado damage analysis overlaid on Census tracts, including surveyed damage path and damage observations from the May 16, 2025 tornado.",
  home: "ZIP-level Zillow Home Value Index typical home value. ZHVI is Zillow's typical-value estimate, not an average sale price.",
  schoolAgeTotal: "2024 ACS 5-year Census tract count of residents ages 5-17.",
  whiteSchoolAgeChildren: "2024 ACS 5-year Census tract count of residents ages 5-17 identified as non-Hispanic White alone.",
  blackSchoolAgeChildren: "2024 ACS 5-year Census tract count of residents ages 5-17 identified as Black alone.",
  publicSchoolEnrollment: "2024 ACS 5-year Census tract residence-based count of ages 5-17 enrolled in public school, including public schools outside SLPS.",
  privateSchoolEnrollment: "2024 ACS 5-year Census tract residence-based count of ages 5-17 enrolled in private school.",
  daaEnrollment2025Total: "DAA Labs Census Block Group layer using internal SLPS 2025-26 enrollment residence aggregates.",
  daaEnrollmentChangeTotal: "DAA Labs Census Block Group layer showing internal SLPS enrollment change from 2024-25 to 2025-26.",
  daaPopulation2020SchoolAge: "DAA Labs Census Block Group layer using 2020 Census counts for residents ages 5-17.",
  daaEnrollmentPerSchoolAge: "Census Block Group comparison of internal SLPS 2025-26 enrollment to 2020 Census school-age population; this mixes source concepts and years.",
};

const backgroundWaffleDescriptions = {
  none: "No background waffle glyphs are drawn.",
  schoolAgeRace: "2024 ACS 5-year Census tract resident race estimates for ages 5-17, drawn as count-based waffle glyphs.",
  publicPrivateSchoolEnrollment: "2024 ACS 5-year Census tract residence-based public and private school enrollment for ages 5-17, drawn as count-based waffle glyphs.",
};

const deepPovertyTooltip = "Deep poverty uses the district direct-certification count: students whose household participation in assistance programs directly certifies them for free meals. Not deep poverty is enrollment minus that count.";

const colors = {
  attendance: ["#173f35", "#6f8f7a", "#d9e4cf"],
  poverty: ["#762a83", "#d8c4dc"],
  race: ["#2f3542", "#f1efe7", "#3f88c5"],
  schoolAgeRace: {
    black: "#2f3542",
    white: "#f1efe7",
    other: "#3f88c5",
  },
  publicPrivateEnrollment: {
    public: "#2f7d32",
    private: "#ff7400",
  },
  enrollmentGrowth: {
    growing: "#2f7d32",
    steady: "#0b5cad",
    shrinking: "#b42318",
    unavailable: "#d6d9dd",
  },
  pto: {
    active: "#008b78",
    possible: "#d18b00",
    exists_activity_unclear: "#d18b00",
    no_public_evidence: "#7d8790",
    not_applicable: "#b8c0c7",
    unknown: "#eef1f3",
  },
  assessment: {
    below_basic: "#b42318",
    basic: "#e5a50a",
    proficient: "#2f7d32",
    advanced: "#0b5cad",
    missing: "#b8c0c7",
    iep: "#7c3aed",
    non_iep: "#94a3b8",
    direct_certified: "#762a83",
    not_direct_certified: "#d8c4dc",
  },
  groups: { neighborhood: "#184e77", magnet_gifted: "#c94833", specialized: "#6f4e7c" },
  sectors: { slps: "#184e77", charter: "#b45309" },
  privateSchool: "#111827",
  closure: "#111827",
};

const ptoStatuses = {
  active: { label: "Active", description: "Public evidence of a school-level PTO" },
  possible: { label: "Possible", description: "PTO evidence exists; activity not fully verified" },
  exists_activity_unclear: { label: "Exists", description: "PTO evidence exists; activity not fully verified" },
  no_public_evidence: { label: "Not found", description: "Research found no school-level PTO evidence" },
  not_applicable: { label: "Not applicable", description: "PTO status is not applicable to this school setting" },
  unknown: { label: "Unknown", description: "Not yet researched" },
};

const state = {
  schoolMode: "attendance",
  schoolLevel: "all",
  closurePlan: "none",
  schoolGlyphScale: 1.75,
  backgroundColoring: "schoolAgeTotal",
  backgroundWaffle: "none",
  showRegions: false,
  showNeighborhoods: false,
  showPrivateSchools: false,
  schoolSectors: { slps: true, charter: false },
  sidebarComparison: "selection",
  legendCollapsed: false,
  selectedSchoolKey: null,
  selectedPrivateSchoolKey: null,
  transform: d3.zoomIdentity,
};

const svg = d3.select("#map");
const loading = d3.select("#loading");
const tooltip = d3.select("#tooltip");
const selection = d3.select("#selection");
const legendPanel = d3.select("#mapLegendPanel");
const legend = d3.select("#legend");
const legendToggle = d3.select("#legendToggle");
const feedbackLink = document.querySelector("#feedbackLink");
const feedbackModal = document.querySelector("#feedbackModal");
const feedbackClose = document.querySelector("#feedbackClose");
let width = 0;
let height = 0;
let projection;
let path;
let root;
let assets;

function formatNumber(value) {
  if (value === null || value === undefined || !Number.isFinite(value)) return "n/a";
  return d3.format(",.0f")(value);
}

function formatPct(value) {
  if (value === null || value === undefined || !Number.isFinite(value)) return "n/a";
  return d3.format(".0%")(value);
}

function formatMapPct(value) {
  if (value === null || value === undefined || !Number.isFinite(value)) return "n/a";
  return `${d3.format(".1f")(value)}%`;
}

function formatTooltipValue(value, share) {
  const shareText = Number.isFinite(share) ? ` (${formatPct(share)})` : "";
  return `${formatNumber(value)}${shareText}`;
}

function formatMoney(value) {
  if (value === null || value === undefined || !Number.isFinite(value)) return "n/a";
  return d3.format("$,.0f")(value);
}

function acsYear() {
  return assets?.metadata?.demographics_metadata?.acs_year || "2024";
}

function acsDatasetLabel() {
  return assets?.metadata?.demographics_metadata?.acs_dataset || `${acsYear()} ACS 5-year`;
}

function acsGeographyLabel() {
  const geography = assets?.metadata?.demographics_metadata?.geography || "tract";
  return geography === "block-group" ? "Census block group" : "Census tract";
}

function acsSourceNote() {
  return `${acsDatasetLabel()} ${acsGeographyLabel()} estimate`;
}

function zhviMonthLabel() {
  const month = assets?.metadata?.demographics_metadata?.zhvi_month;
  return month ? month.slice(0, 7) : null;
}

function zhviSourceNote() {
  const month = zhviMonthLabel();
  return `Zillow ZHVI${month ? ` ${month}` : ""}`;
}

const daaBackgroundLayers = {
  daaEnrollment2025Total: {
    property: "slps_enrollment_2025_26_total",
    title: "SLPS Enrollment 2025-26 (DAA)",
    formatter: formatNumber,
    interpolator: d3.interpolateYlGnBu,
    scale: "sequential",
    detail: "Internal SLPS 2025-26 enrollment by Census Block Group.",
  },
  daaEnrollmentChangeTotal: {
    property: "slps_enrollment_change_total",
    title: "SLPS Enrollment Change 2024-25 to 2025-26 (DAA)",
    formatter: formatNumber,
    interpolator: d3.interpolateRdBu,
    scale: "diverging",
    detail: "2025-26 minus 2024-25 internal SLPS enrollment by Census Block Group.",
  },
  daaPopulation2020SchoolAge: {
    property: "census_2020_ages_5_17",
    title: "2020 School-Age Population (Census)",
    formatter: formatNumber,
    interpolator: d3.interpolateYlOrBr,
    scale: "sequential",
    detail: "2020 Census residents ages 5-17 by Census Block Group.",
  },
  daaEnrollmentPerSchoolAge: {
    property: "slps_enrollment_2025_26_per_2020_school_age",
    title: "SLPS 2025-26 Enrollment per 2020 School-Age Population",
    formatter: formatPct,
    interpolator: d3.interpolateBuGn,
    scale: "sequential",
    detail: "Compares internal SLPS 2025-26 enrollment with 2020 Census ages 5-17, mixing sources and years.",
  },
};

function daaBackgroundConfig(id = state.backgroundColoring) {
  return daaBackgroundLayers[id] || null;
}

function formatDetailValue(value, total) {
  return `${formatNumber(value)}${total > 0 ? ` (${formatPct((value || 0) / total)})` : ""}`;
}

function escapeHtml(value) {
  return String(value ?? "")
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function ptoStatusMeta(status) {
  if (status === "not_found") return ptoStatuses.no_public_evidence;
  return ptoStatuses[status] || ptoStatuses.unknown;
}

function ptoStatusGlyph(status, meta) {
  if (status === "not_found" || status === "no_public_evidence") return "?";
  return meta.label[0];
}

function ptoSourceHtml(school) {
  const label = school.pto_source_label || school.pto_source_url;
  if (!label) return "n/a";
  if (!school.pto_source_url) return escapeHtml(label);
  return `<a href="${escapeHtml(school.pto_source_url)}" target="_blank" rel="noopener noreferrer">${escapeHtml(label)}</a>`;
}

function linkHtml(url, label = url) {
  return `<a href="${escapeHtml(url)}" target="_blank" rel="noopener noreferrer">${escapeHtml(label)}</a>`;
}

function sourceLinkLabel(source) {
  try {
    return new URL(source).hostname.replace(/^www\./i, "");
  } catch {
    return "Source";
  }
}

function sourceRowsHtml() {
  const sources = assets.metadata.sources || {};
  const demographics = assets.metadata.demographics_metadata || {};
  const rows = [
    ["School list and metrics", sources.schools],
    ["Future Ready planning matrix", sources.future_ready_matrix],
    ["PTO activity research", sources.pto_activity],
    ["Missouri Assessment Program 2025", sources.map_2025],
    ["Charter school campuses", sources.charter_schools_edge],
    ["Charter school cache", sources.charter_schools_edge_cache],
    ["Private school locations", sources.private_schools_edge],
    ["Private school PSS enrichment", sources.private_schools_pss_cache],
    ["DAA Labs source", sources.daa_labs],
    ["DAA Labs enrollment workbook", sources.daa_labs_workbooks?.enrollment],
    ["DAA Labs population workbook", sources.daa_labs_workbooks?.population],
    ["Neighborhood boundaries", sources.neighborhoods],
    ["Elementary assignment regions", sources.regions?.elementary],
    ["Middle assignment regions", sources.regions?.middle],
    ["High assignment regions", sources.regions?.high],
    ["Zillow ZHVI typical home value", demographics.zillow_url],
    ["2025 tornado damage analysis", demographics.tornado_source],
    ["St. Louis city boundary", demographics.city_boundary_url],
    ["St. Louis ZIP geometry", demographics.city_zip_url],
    ["Census TIGER tract geometry", demographics.census_tiger_url],
  ].filter(([, source]) => source);

  return rows.map(([label, source]) => {
    const sourceText = /^https?:\/\//i.test(source) ? linkHtml(source, sourceLinkLabel(source)) : escapeHtml(source);
    return `<li><span>${escapeHtml(label)}</span><strong style="word-break:break-word;white-space:wrap">${sourceText}</strong></li>`;
  }).join("");
}

function roundedSquares(items, unit = 20) {
  const positive = items.filter((d) => d.value > 0);
  if (!positive.length) return [];
  const target = Math.max(1, Math.round(d3.sum(positive, (d) => d.value) / unit));
  const floors = positive.map((d, index) => ({
    ...d,
    order: index,
    count: Math.floor(d.value / unit),
    remainder: d.value / unit - Math.floor(d.value / unit),
  }));
  let remaining = Math.max(0, target - d3.sum(floors, (d) => d.count));
  for (const item of [...floors].sort((a, b) => d3.descending(a.remainder, b.remainder))) {
    if (!remaining) break;
    item.count += 1;
    remaining -= 1;
  }
  return floors
    .sort((a, b) => d3.ascending(a.order, b.order))
    .flatMap((item) => d3.range(item.count).map(() => item));
}

function finiteNumber(value) {
  return Number.isFinite(value) ? value : 0;
}

function displaySafeCount(value) {
  return Math.max(0, finiteNumber(value));
}

function schoolAgeRaceRows(properties) {
  const total = displaySafeCount(properties.school_age_5_17);
  const rows = [
    {
      key: "Black alone",
      value: displaySafeCount(properties.school_age_black_alone_5_17),
      share: properties.school_age_black_alone_5_17_share,
      color: colors.schoolAgeRace.black,
    },
    {
      key: "White, non-Hispanic",
      value: displaySafeCount(properties.school_age_white_non_hispanic_5_17),
      share: properties.school_age_white_non_hispanic_5_17_share,
      color: colors.schoolAgeRace.white,
    },
    {
      key: "Other / residual",
      value: displaySafeCount(properties.school_age_other_or_unclassified_5_17),
      share: properties.school_age_other_or_unclassified_5_17_share,
      color: colors.schoolAgeRace.other,
    },
  ];
  return { total, rows };
}

function publicPrivateEnrollmentRows(properties) {
  const rows = [
    {
      key: "Public school",
      value: displaySafeCount(properties.public_school_enrolled_5_17),
      color: colors.publicPrivateEnrollment.public,
    },
    {
      key: "Private school",
      value: displaySafeCount(properties.private_school_enrolled_5_17),
      color: colors.publicPrivateEnrollment.private,
    },
  ];
  return { total: d3.sum(rows, (row) => row.value), rows };
}

const assessmentContentOrder = ["ela", "math", "science", "social_studies"];

function gradeLabel(grade) {
  if (!grade) return "n/a";
  if (grade === "all") return "All grades";
  return /^\d+$/.test(String(grade)) ? `Grade ${grade}` : String(grade);
}

function bestAssessmentContent(assessment, sectionKey) {
  const content = assessment?.content || {};
  return assessmentContentOrder
    .map((key) => [key, content[key]])
    .find(([, value]) => value?.[sectionKey]?.default_grade)?.[1] || null;
}

function bestAssessmentPerformanceContent(assessment) {
  const content = assessment?.content || {};
  return assessmentContentOrder
    .map((key) => content[key])
    .find((value) => {
      const grade = value?.all_students?.default_grade;
      const record = grade ? value.all_students.grades[grade] : null;
      return record && Number.isFinite(record.proficient_or_advanced_pct) && assessmentPerformanceRows(record).length;
    }) || null;
}

function assessmentPerformanceRows(record) {
  if (!record || !Number.isFinite(record.n_size) || record.n_size <= 0) return [];
  return [
    { key: "Below Basic", pct: record.below_basic_pct, color: colors.assessment.below_basic },
    { key: "Basic", pct: record.basic_pct, color: colors.assessment.basic },
    { key: "Proficient", pct: record.proficient_pct, color: colors.assessment.proficient },
    { key: "Advanced", pct: record.advanced_pct, color: colors.assessment.advanced },
    { key: "Missing", pct: record.missing_pct, color: colors.assessment.missing },
  ]
    .filter((row) => Number.isFinite(row.pct) && row.pct > 0)
    .map((row) => ({
      ...row,
      value: record.n_size * row.pct / 100,
    }));
}

function assessmentMarkerPerformance(school) {
  const content = bestAssessmentPerformanceContent(school.assessment_2025);
  if (!content) return null;
  const grade = content.all_students.default_grade;
  const record = content.all_students.grades[grade];
  const rows = assessmentPerformanceRows(record);
  if (!rows.length || !Number.isFinite(record.proficient_or_advanced_pct)) return null;
  return {
    rows,
    unit: 20,
    label: `${content.label} / ${gradeLabel(grade)}`,
    nSize: record.n_size,
    proficientOrAdvanced: record.proficient_or_advanced_pct,
    valueKind: "performance_count",
  };
}

function assessmentMarkerIep(school) {
  const aggregate = school.assessment_2025?.iep_composition_aggregate;
  if (aggregate) {
    const rows = [
      { key: "Students with an IEP", value: aggregate.iep_n_size, color: colors.assessment.iep },
      { key: "Students without an IEP", value: aggregate.non_iep_n_size, color: colors.assessment.non_iep },
    ];
    const total = d3.sum(rows, (row) => row.value || 0);
    if (total) {
      return {
        rows,
        unit: 20,
        label: `${formatNumber(aggregate.cell_count)} available MAP-tested cells`,
        nSize: aggregate.all_students_n_size || total,
        valueKind: "count",
        note: aggregate.derived_cell_count
          ? "Aggregated MAP-tested records across available content/grade cells; some IEP counts are derived from All Students minus Non IEP. Not unique students or full-school enrollment."
          : "Aggregated MAP-tested records across available paired content/grade cells, not unique students or full-school enrollment.",
      };
    }
  }
  const content = bestAssessmentContent(school.assessment_2025, "iep_composition");
  if (!content) return null;
  const grade = content.iep_composition.default_grade;
  const record = content.iep_composition.grades[grade];
  const rows = [
    { key: "Students with an IEP", value: record.iep_n_size, color: colors.assessment.iep },
    { key: "Students without an IEP", value: record.non_iep_n_size, color: colors.assessment.non_iep },
  ];
  const total = d3.sum(rows, (row) => row.value || 0);
  if (!total) return null;
  return {
    rows,
    unit: 20,
    label: `${content.label} / ${gradeLabel(grade)}`,
    nSize: record.all_students_n_size || total,
    valueKind: "count",
    note: "MAP-tested student composition, not full-school enrollment.",
  };
}

function assessmentMarkerData(school, mode = state.schoolMode) {
  if ((mode === "assessmentPerformance" && !hasMetric(school, "map_performance"))
    || (mode === "assessmentIep" && !hasMetric(school, "iep_distribution"))) {
    return null;
  }
  if (mode === "assessmentPerformance") return assessmentMarkerPerformance(school);
  if (mode === "assessmentIep") return assessmentMarkerIep(school);
  return null;
}

function sectorLabel(schoolOrSector) {
  const sector = typeof schoolOrSector === "string" ? schoolOrSector : schoolOrSector?.sector;
  if (sector === "charter") return "Charter";
  if (sector === "private") return "Private";
  return "SLPS";
}

function hasMetric(school, key) {
  if (!school) return false;
  if (school.availability && key in school.availability) return Boolean(school.availability[key]);
  return school.sector !== "charter";
}

function slpsSchools(schools) {
  return schools.filter((school) => school.sector !== "charter");
}

function schoolsWithMetric(schools, key) {
  return schools.filter((school) => hasMetric(school, key));
}

function enrollmentGrowthData(school) {
  const history = Array.isArray(school?.enrollment_history_10yr) ? school.enrollment_history_10yr : [];
  const maxEnrollment = Number(school?.enrollment_history_max);
  if (!school?.availability?.enrollment_history || history.length < 1 || !Number.isFinite(maxEnrollment) || maxEnrollment <= 0) {
    return null;
  }
  return {
    history,
    maxEnrollment,
    direction: school.enrollment_trend_direction || "steady",
    slope: Number(school.enrollment_trend_slope),
    normalizedSlope: Number(school.enrollment_trend_slope_normalized),
    trendAvailable: school.enrollment_trend_available !== false,
  };
}

function enrollmentGrowthColor(school) {
  const data = enrollmentGrowthData(school);
  return colors.enrollmentGrowth[data?.direction] || colors.enrollmentGrowth.unavailable;
}

function enrollmentGrowthRangeLabel(data) {
  if (!data?.history?.length) return "n/a";
  return `${data.history[0].year}-${data.history[data.history.length - 1].year}`;
}

function enrollmentGrowthDirectionLabel(direction, trendAvailable = true) {
  if (!trendAvailable) return "n/a";
  if (direction === "growing") return "Growing";
  if (direction === "shrinking") return "Shrinking";
  return "Steady";
}

function enrollmentGrowthWindowLabel(data) {
  if (!data?.history?.length) return "Enrollment history";
  return `${data.history.length}-year enrollment history`;
}

function schoolCategories(school, mode = state.schoolMode) {
  if (school?.sector === "private") {
    return privateSchoolGlyphRows(school, mode).rows;
  }
  if (mode === "attendance") {
    if (!hasMetric(school, "attendance") && !hasMetric(school, "capacity")) {
      return [
        { key: "Campus enrollment", value: school.enrollment_2025 || school.campus_enrollment || 0, color: colors.sectors.charter },
      ];
    }
    return [
      { key: "90%+ attendance", value: school.attendance_90plus_students || 0, color: colors.attendance[0] },
      { key: "Enrolled below cutoff", value: school.attendance_below_90_students || 0, color: colors.attendance[1] },
      { key: "Empty capacity", value: school.empty_capacity_seats || 0, color: colors.attendance[2] },
    ];
  }
  if (mode === "poverty") {
    return [
      { key: "Direct-certified", value: school.direct_cert_count || 0, color: colors.poverty[0], description: deepPovertyTooltip },
      { key: "Not direct-certified", value: school.not_direct_cert_count || 0, color: colors.poverty[1], description: deepPovertyTooltip },
    ];
  }
  if (mode === "assessmentPerformance" || mode === "assessmentIep") {
    return assessmentMarkerData(school, mode)?.rows || [];
  }
  return [
    { key: "Black", value: school.black_count || 0, color: colors.race[0] },
    { key: "White", value: school.white_count || 0, color: colors.race[1] },
    { key: "Other", value: school.other_count || 0, color: colors.race[2] },
  ];
}

function selectedClosureStatus(school) {
  if (school?.sector === "charter") return null;
  if (state.closurePlan === "none") return null;
  return school[state.closurePlan] || null;
}

function isClosedUnderSelectedPlan(school) {
  const status = selectedClosureStatus(school);
  return Boolean(status && /closed|repurposed/i.test(status));
}

function gradeTokenValue(token) {
  const value = String(token || "").trim().toLowerCase();
  if (value === "pk" || value === "pre-k" || value === "prek") return -1;
  if (value === "k" || value === "kg" || value === "kn") return 0;
  if (/^\d+$/.test(value)) return Number(value);
  return null;
}

function gradeSpanRange(gradeSpan) {
  const matches = String(gradeSpan || "").match(/\b(?:pre-k|prek|pk|kg|kn|k|\d{1,2})\b/gi) || [];
  const values = matches
    .map(gradeTokenValue)
    .filter((value) => Number.isFinite(value));
  if (!values.length) return null;
  return { low: Math.min(...values), high: Math.max(...values) };
}

function gradeSpanMatchesLevel(gradeSpan, level) {
  const range = gradeSpanRange(gradeSpan);
  if (!range) return null;
  if (level === "elementary") return range.low <= 1 && range.high >= 1;
  if (level === "middle") return range.low <= 8 && range.high >= 6;
  if (level === "high") return range.low <= 12 && range.high >= 9;
  return false;
}

function schoolGradeSpan(school) {
  return school.current_grade_span || school.grade_span || "";
}

function gradeSpanTooltipHtml(school) {
  const gradeSpan = schoolGradeSpan(school);
  if (!gradeSpan) return "";
  const label = /^\s*grades?\b/i.test(String(gradeSpan)) ? gradeSpan : `Grades ${gradeSpan}`;
  return `<div>${escapeHtml(label)}</div>`;
}

function schoolLevelMatches(school) {
  if (state.schoolLevel === "none") return false;
  if (state.schoolLevel === "all") return true;
  const gradeSpanMatch = gradeSpanMatchesLevel(schoolGradeSpan(school), state.schoolLevel);
  if (gradeSpanMatch !== null) return gradeSpanMatch;
  const text = [
    school.school_level || school.school_type,
    school.school_type_text,
  ].filter(Boolean).join(" ").toLowerCase();
  if (state.schoolLevel === "elementary") return /elementary|primary|pk|kg|\bk\b/.test(text) && !/\bhigh\b|secondary/.test(text);
  if (state.schoolLevel === "middle") return /middle/.test(text);
  if (state.schoolLevel === "high") return /\bhigh\b|secondary/.test(text);
  return false;
}

function schoolSectorMatches(school) {
  return Boolean(state.schoolSectors[school.sector || "slps"]);
}

function visibleSchools() {
  return assets.schools.schools.filter((school) => schoolSectorMatches(school) && schoolLevelMatches(school));
}

function visiblePrivateSchools() {
  return assets.privateSchools.schools.filter(schoolLevelMatches);
}

function visibleSummaryPrivateSchools() {
  return state.showPrivateSchools ? visiblePrivateSchools() : [];
}

function schoolKey(school) {
  return school.key || school.school_code || school.school_name;
}

function selectedSchool() {
  if (!state.selectedSchoolKey || !assets) return null;
  return assets.schools.schools.find((school) => schoolKey(school) === state.selectedSchoolKey) || null;
}

function selectedPrivateSchool() {
  if (!state.selectedPrivateSchoolKey || !assets) return null;
  return assets.privateSchools.schools.find((school) => school.key === state.selectedPrivateSchoolKey) || null;
}

function schoolSearchLabel(item) {
  const school = item.school;
  if (item.kind === "private") {
    return `${school.school_name} (Private${schoolGradeSpan(school) ? `, ${schoolGradeSpan(school)}` : ""})`;
  }
  const sector = sectorLabel(school);
  const gradeSpan = schoolGradeSpan(school);
  return `${school.school_name} (${sector}${gradeSpan ? `, ${gradeSpan}` : ""})`;
}

function schoolSearchOptions() {
  if (!assets) return [];
  const publicOptions = visibleSchools().map((school) => ({ kind: "public", key: schoolKey(school), school }));
  const privateOptions = visibleSummaryPrivateSchools().map((school) => ({ kind: "private", key: school.key, school }));
  return publicOptions
    .concat(privateOptions)
    .sort((a, b) => d3.ascending(a.school.school_name, b.school.school_name) || d3.ascending(a.kind, b.kind));
}

function summarySchoolSearchHtml() {
  const options = schoolSearchOptions();
  if (!options.length) return "";
  return `
    <div class="summary-school-search">
      <label for="summarySchoolSearch">Find a school</label>
      <input id="summarySchoolSearch" type="search" list="summarySchoolSearchOptions" autocomplete="off" placeholder="Type a school name" />
      <datalist id="summarySchoolSearchOptions">
        ${options.map((option) => `<option value="${escapeHtml(schoolSearchLabel(option))}"></option>`).join("")}
      </datalist>
    </div>
  `;
}

function selectSearchOption(option) {
  if (!option) return;
  if (option.kind === "private") selectPrivateSchool(option.school);
  else selectSchool(option.school);
}

function bindSummarySchoolSearch() {
  const input = d3.select("#summarySchoolSearch");
  if (input.empty()) return;
  const options = schoolSearchOptions();
  const optionForValue = (value) => {
    const normalized = String(value || "").trim().toLowerCase();
    if (!normalized) return null;
    return options.find((option) => schoolSearchLabel(option).toLowerCase() === normalized)
      || options.find((option) => option.school.school_name.toLowerCase() === normalized)
      || null;
  };
  const firstMatch = (value) => {
    const normalized = String(value || "").trim().toLowerCase();
    if (!normalized) return null;
    return optionForValue(value)
      || options.find((option) => schoolSearchLabel(option).toLowerCase().includes(normalized)
        || option.school.school_name.toLowerCase().includes(normalized))
      || null;
  };
  input
    .on("change", (event) => selectSearchOption(optionForValue(event.target.value)))
    .on("keydown", (event) => {
      if (event.key !== "Enter") return;
      const match = firstMatch(event.target.value);
      if (!match) return;
      event.preventDefault();
      selectSearchOption(match);
    });
}

function refreshSelection() {
  if (state.sidebarComparison !== "selection") {
    renderSidebarComparison();
    return;
  }
  const privateSchool = selectedPrivateSchool();
  if (privateSchool && state.showPrivateSchools && schoolLevelMatches(privateSchool)) {
    renderSelectedPrivateSchool(privateSchool);
    return;
  }
  const school = selectedSchool();
  if (school && schoolLevelMatches(school)) {
    renderSelectedSchool(school);
    return;
  }
  if (school) state.selectedSchoolKey = null;
  if (privateSchool) state.selectedPrivateSchoolKey = null;
  renderSummarySelection();
}

function initControls() {
  d3.select("#schoolMetricControls")
    .selectAll("button")
    .data(schoolModes)
    .join("button")
    .attr("type", "button")
    .attr("role", "radio")
    .attr("aria-checked", (d) => d.id === state.schoolMode)
    .text((d) => d.label)
    .on("click", (_, d) => {
      state.schoolMode = d.id;
      updateControls();
      renderSchools();
      renderPrivateSchools();
      renderLegend();
      refreshSelection();
    });

  d3.select("#schoolLevelControls")
    .selectAll("button")
    .data(schoolLevels)
    .join("button")
    .attr("type", "button")
    .attr("role", "radio")
    .attr("aria-checked", (d) => d.id === state.schoolLevel)
    .text((d) => d.label)
    .on("click", (_, d) => {
      state.schoolLevel = d.id;
      if (state.schoolLevel === "all" || state.schoolLevel === "none") state.showRegions = false;
      if (state.schoolLevel === "none") {
        state.closurePlan = "none";
        state.selectedSchoolKey = null;
      }
      updateControls();
      renderSchools();
      renderPrivateSchools();
      renderRegions();
      renderLegend();
      renderMetadata();
      refreshSelection();
    });

  d3.select("#schoolSectorControls")
    .selectAll("label")
    .data(schoolSectors)
    .join("label")
    .attr("class", "toggle")
    .html((d) => `<input type="checkbox" data-sector="${escapeHtml(d.id)}" ${state.schoolSectors[d.id] ? "checked" : ""} /> <span>${escapeHtml(d.label)}</span>`)
    .select("input")
    .on("change", (event, d) => {
      state.schoolSectors[d.id] = event.target.checked;
      if (!visibleSchools().some((school) => schoolKey(school) === state.selectedSchoolKey)) {
        state.selectedSchoolKey = null;
      }
      updateControls();
      renderSchools();
      renderLegend();
      renderMetadata();
      refreshSelection();
    });

  d3.select("#closurePlanControls")
    .selectAll("button")
    .data(closurePlans)
    .join("button")
    .attr("type", "button")
    .attr("role", "radio")
    .attr("aria-checked", (d) => d.id === state.closurePlan)
    .text((d) => d.label)
    .on("click", (_, d) => {
      state.closurePlan = d.id;
      updateControls();
      renderSchools();
      renderLegend();
      renderMetadata();
      refreshSelection();
    });

  const bgColor = d3.select("#backgroundColoringControls");
  bgColor.selectAll("button")
    .data(backgroundColorings)
    .join("button")
    .attr("type", "button")
    .attr("role", "radio")
    .attr("aria-checked", (d) => d.id === state.backgroundColoring)
    .text((d) => d.label)
    .on("click", (_, d) => {
      state.backgroundColoring = d.id;
      updateControls();
      renderBackground();
      renderLegend();
    });

  const bgWaffle = d3.select("#backgroundWaffleControls");
  bgWaffle.selectAll("button")
    .data(backgroundWaffles)
    .join("button")
    .attr("type", "button")
    .attr("role", "radio")
    .attr("aria-checked", (d) => d.id === state.backgroundWaffle)
    .text((d) => d.label)
    .on("click", (_, d) => {
      state.backgroundWaffle = d.id;
      updateControls();
      renderBackground();
      renderLegend();
    });

  d3.select("#backgroundNotes")
    .append("div")
    .attr("id", "backgroundColoringDescription")
    .attr("class", "background-note");
  d3.select("#backgroundNotes")
    .append("div")
    .attr("id", "backgroundWaffleDescription")
    .attr("class", "background-note");

  d3.select("#sidebarComparisonControls")
    .selectAll("button")
    .data(sidebarComparisons)
    .join("button")
    .attr("type", "button")
    .attr("role", "radio")
    .attr("aria-checked", (d) => d.id === state.sidebarComparison)
    .text((d) => d.label)
    .on("click", (_, d) => {
      state.sidebarComparison = d.id;
      updateControls();
      refreshSelection();
    });

  d3.select("#schoolGlyphSize").on("input", (event) => {
    state.schoolGlyphScale = Number(event.target.value);
    updateControls();
    positionSchools();
    positionPrivateSchools();
  });
  d3.select("#showRegions").on("change", (event) => {
    state.showRegions = event.target.checked;
    renderRegions();
  });
  d3.select("#showNeighborhoods").on("change", (event) => {
    state.showNeighborhoods = event.target.checked;
    renderLabels();
  });
  d3.select("#showPrivateSchools").on("change", (event) => {
    state.showPrivateSchools = event.target.checked;
    if (!state.showPrivateSchools) state.selectedPrivateSchoolKey = null;
    updateControls();
    renderPrivateSchools();
    renderLegend();
    renderMetadata();
    refreshSelection();
  });
  legendToggle.on("click", () => {
    state.legendCollapsed = !state.legendCollapsed;
    updateLegendCollapse();
  });
  updateControls();
  updateLegendCollapse();
}

function initFeedbackModal() {
  if (!feedbackLink || !feedbackModal || !feedbackClose) return;
  feedbackLink.addEventListener("click", () => feedbackModal.showModal());
  window.showFeedbackModal = () => feedbackModal.showModal()
  feedbackClose.addEventListener("click", () => feedbackModal.close());
  feedbackModal.addEventListener("click", (event) => {
    if (event.target === feedbackModal) feedbackModal.close();
  });
}

function updateControls() {
  d3.select(".app-shell").classed("is-comparing", state.sidebarComparison !== "selection");
  d3.select("#schoolMetricControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.schoolMode);
  d3.select("#schoolLevelControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.schoolLevel);
  d3.select("#schoolSectorControls")
    .selectAll("input")
    .property("checked", (d) => state.schoolSectors[d.id]);
  d3.select("#closurePlanControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.closurePlan);
  d3.select("#backgroundColoringControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.backgroundColoring);
  d3.select("#backgroundWaffleControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.backgroundWaffle);
  d3.select("#sidebarComparisonControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.sidebarComparison);
  d3.select("#showRegions")
    .property("checked", state.showRegions)
    .property("disabled", state.schoolLevel === "all" || state.schoolLevel === "none");
  d3.select("#closurePlanControls")
    .selectAll("button")
    .property("disabled", state.schoolLevel === "none");
  d3.select("#showPrivateSchools").property("checked", state.showPrivateSchools);
  d3.select("#schoolGlyphSizeValue").text(`${d3.format(".2~f")(state.schoolGlyphScale)}x`);
  d3.select("#backgroundColoringDescription").text(backgroundColoringDescriptions[state.backgroundColoring] || "");
  d3.select("#backgroundWaffleDescription").text(backgroundWaffleDescriptions[state.backgroundWaffle] || "");
  const selectedMode = schoolModes.find((d) => d.id === state.schoolMode);
  d3.select("#schoolMetricDescription").text(selectedMode?.description || "");
}

function fitMap() {
  const rect = svg.node().getBoundingClientRect();
  width = Math.max(320, rect.width);
  height = Math.max(360, rect.height);
  svg.attr("viewBox", [0, 0, width, height]);
  projection = d3.geoMercator().fitSize([width, height], assets.neighborhoods);
  path = d3.geoPath(projection);
}

function buildScene() {
  svg.selectAll("*").remove();
  root = svg.append("g").attr("class", "scene");
  root.append("g").attr("class", "background-layer");
  root.append("g").attr("class", "neighborhood-layer");
  root.append("g").attr("class", "region-layer");
  root.append("g").attr("class", "tornado-layer");
  root.append("g").attr("class", "label-layer");
  root.append("g").attr("class", "school-layer");
  root.append("g").attr("class", "private-school-layer");

  svg.call(
    d3.zoom()
      .scaleExtent([0.5, 15])
      .on("zoom", (event) => {
        state.transform = event.transform;
        root.attr("transform", state.transform);
        root.selectAll(".label").style("font-size", `${Math.max(4, 10 / Math.sqrt(state.transform.k))}px`);
        root.selectAll(".region").style("stroke-width", 1.8 / state.transform.k);
        root.selectAll(".neighborhood").style("stroke-width", 0.65 / state.transform.k);
        positionSchools();
        positionPrivateSchools();
      })
  );
}

function renderNeighborhoods() {
  const layer = root.select(".neighborhood-layer");
  layer.selectAll("*").remove();
  layer
    .selectAll("path")
    .data(assets.neighborhoods.features)
    .join("path")
    .attr("class", "neighborhood")
    .attr("d", path);
}

function renderBackground() {
  const layer = root.select(".background-layer");
  layer.selectAll("*").remove();
  root.select(".tornado-layer").selectAll("*").remove();

  if (state.backgroundColoring === "none" && state.backgroundWaffle === "none") return;

  if (state.backgroundColoring === "home") {
    const values = assets.zips.features.map((d) => d.properties.zhvi_typical_home_value).filter(Number.isFinite);
    const extent = d3.extent(values);
    const color = d3.scaleSequential(d3.interpolateYlGnBu)
      .domain([extent[0], HOME_VALUE_SCALE_MAX])
      .clamp(true);
    layer.selectAll("path")
      .data(assets.zips.features)
      .join("path")
      .attr("class", "demographic")
      .attr("fill", (d) => color(d.properties.zhvi_typical_home_value))
      .attr("d", path)
      .on("mousemove", (event, d) => showTooltip(event, `<strong>ZIP ${d.properties.city_zip}</strong><br>${escapeHtml(zhviSourceNote())}: ${formatMoney(d.properties.zhvi_typical_home_value)}`))
      .on("mouseleave", hideTooltip);
    renderBackgroundWaffles(layer, false);
    return;
  }

  const daaConfig = daaBackgroundConfig();
  if (daaConfig) {
    const features = assets.daaBlockGroups?.features || [];
    const values = features.map((d) => d.properties[daaConfig.property]).filter(Number.isFinite);
    const color = daaConfig.scale === "diverging"
      ? d3.scaleDiverging(daaConfig.interpolator).domain([-(d3.max(values.map(Math.abs)) || 1), 0, d3.max(values.map(Math.abs)) || 1])
      : d3.scaleSequential(daaConfig.interpolator).domain([0, d3.max(values) || 1]);
    layer.selectAll("path")
      .data(features)
      .join("path")
      .attr("class", "demographic")
      .attr("fill", (d) => Number.isFinite(d.properties[daaConfig.property]) ? color(d.properties[daaConfig.property]) : "#e5e7eb")
      .attr("d", path)
      .on("mousemove", (event, d) => showTooltip(event, backgroundColoringTooltip(d)))
      .on("mouseleave", hideTooltip);
    renderBackgroundWaffles(layer, false);
    return;
  }

  const features = assets.demographics.features;
  if (state.backgroundColoring === "none") {
    renderBackgroundWaffles(layer, true);
    return;
  }

  let value;
  let color;
  if (state.backgroundColoring === "race") {
    value = (d) => d.properties.race_black_share;
    color = d3.scaleSequential(d3.interpolatePuBuGn).domain([0, 1]);
  } else if (state.backgroundColoring === "poverty") {
    value = (d) => d.properties.poverty_rate;
    color = d3.scaleSequential(d3.interpolateOrRd).domain([0, d3.max(features, value) || 1]);
  } else if (state.backgroundColoring === "schoolAgeTotal") {
    value = (d) => d.properties.school_age_5_17;
    color = d3.scaleSequential(d3.interpolateYlOrBr).domain([0, d3.max(features, value) || 1]);
  } else if (state.backgroundColoring === "whiteSchoolAgeChildren") {
    value = (d) => d.properties.school_age_white_non_hispanic_5_17;
    color = d3.scaleSequential(d3.interpolateGreens).domain([0, d3.max(features, value) || 1]);
  } else if (state.backgroundColoring === "blackSchoolAgeChildren") {
    value = (d) => d.properties.school_age_black_alone_5_17;
    color = d3.scaleSequential(d3.interpolatePuBu).domain([0, d3.max(features, value) || 1]);
  } else if (state.backgroundColoring === "publicSchoolEnrollment") {
    value = (d) => d.properties.public_school_enrolled_5_17;
    color = d3.scaleSequential(d3.interpolateGreens).domain([0, d3.max(features, value) || 1]);
  } else if (state.backgroundColoring === "privateSchoolEnrollment") {
    value = (d) => d.properties.private_school_enrolled_5_17;
    color = d3.scaleSequential(d3.interpolateOranges).domain([0, d3.max(features, value) || 1]);
  } else if (state.backgroundColoring === "tornado") {
    value = (d) => d.properties.tornado_damage_area_share;
    color = d3.scaleSequential(d3.interpolateReds).domain([0, d3.max(features, value) || 1]);
    renderTornado();
  }

  layer.selectAll("path")
    .data(features)
    .join("path")
    .attr("class", "demographic")
    .attr("fill", (d) => color(value(d) || 0))
    .attr("d", path)
    .on("mousemove", (event, d) => showTooltip(event, backgroundColoringTooltip(d)))
    .on("mouseleave", hideTooltip);

  renderBackgroundWaffles(layer, false);
}

function renderBackgroundWaffles(layer, includeNeutralBackground) {
  const features = assets.demographics.features;
  if (state.backgroundWaffle === "schoolAgeRace") {
    renderSchoolAgeRaceBackground(layer, features, includeNeutralBackground);
  } else if (state.backgroundWaffle === "publicPrivateSchoolEnrollment") {
    renderPublicPrivateEnrollmentBackground(layer, features, includeNeutralBackground);
  }
}

function renderSchoolAgeRaceBackground(layer, features, includeNeutralBackground) {
  if (includeNeutralBackground) {
    layer.selectAll("path")
      .data(features)
      .join("path")
      .attr("class", "demographic demographic-neutral")
      .attr("fill", "#eef1f3")
      .attr("d", path)
      .on("mousemove", (event, d) => showTooltip(event, backgroundWaffleTooltip(d)))
      .on("mouseleave", hideTooltip);
  }

  const glyphs = layer.selectAll("g.demographic-race-glyph")
    .data(features.filter((d) => schoolAgeRaceRows(d.properties).rows.some((row) => row.value > 0)))
    .join("g")
    .attr("class", "demographic-race-glyph")
    .attr("transform", (d) => {
      const [x, y] = path.centroid(d);
      return `translate(${x},${y})`;
    })
    .on("mousemove", (event, d) => showTooltip(event, backgroundWaffleTooltip(d)))
    .on("mouseleave", hideTooltip);

  glyphs.each(function(d) {
    const squares = roundedSquares(schoolAgeRaceRows(d.properties).rows, SCHOOL_AGE_RACE_WAFFLE_UNIT);
    const columns = Math.ceil(Math.sqrt(squares.length || 1));
    const size = 3.2;
    const gap = 0.8;
    const pitch = size + gap;
    const width = columns * pitch - gap;
    const height = Math.ceil((squares.length || 1) / columns) * pitch - gap;
    d3.select(this)
      .selectAll("rect")
      .data(squares)
      .join("rect")
      .attr("class", "demographic-waffle-square demographic-race-waffle-square")
      .attr("x", (_, i) => (i % columns) * pitch - width / 2)
      .attr("y", (_, i) => Math.floor(i / columns) * pitch - height / 2)
      .attr("width", size)
      .attr("height", size)
      .attr("rx", 0.7)
      .attr("fill", (row) => row.color);
  });
}

function renderPublicPrivateEnrollmentBackground(layer, features, includeNeutralBackground) {
  if (includeNeutralBackground) {
    layer.selectAll("path")
      .data(features)
      .join("path")
      .attr("class", "demographic demographic-neutral")
      .attr("fill", "#eef1f3")
      .attr("d", path)
      .on("mousemove", (event, d) => showTooltip(event, backgroundWaffleTooltip(d)))
      .on("mouseleave", hideTooltip);
  }

  const glyphs = layer.selectAll("g.demographic-enrollment-glyph")
    .data(features.filter((d) => publicPrivateEnrollmentRows(d.properties).rows.some((row) => row.value > 0)))
    .join("g")
    .attr("class", "demographic-enrollment-glyph")
    .attr("transform", (d) => {
      const [x, y] = path.centroid(d);
      return `translate(${x},${y})`;
    })
    .on("mousemove", (event, d) => showTooltip(event, backgroundWaffleTooltip(d)))
    .on("mouseleave", hideTooltip);

  glyphs.each(function(d) {
    const squares = roundedSquares(publicPrivateEnrollmentRows(d.properties).rows, PUBLIC_PRIVATE_ENROLLMENT_WAFFLE_UNIT);
    const columns = Math.ceil(Math.sqrt(squares.length || 1));
    const size = 3.2;
    const gap = 0.8;
    const pitch = size + gap;
    const width = columns * pitch - gap;
    const height = Math.ceil((squares.length || 1) / columns) * pitch - gap;
    d3.select(this)
      .selectAll("rect")
      .data(squares)
      .join("rect")
      .attr("class", "demographic-waffle-square")
      .attr("x", (_, i) => (i % columns) * pitch - width / 2)
      .attr("y", (_, i) => Math.floor(i / columns) * pitch - height / 2)
      .attr("width", size)
      .attr("height", size)
      .attr("rx", 0.7)
      .attr("fill", (row) => row.color);
  });
}

function renderTornado() {
  const layer = root.select(".tornado-layer");
  layer.selectAll("path.poly")
    .data(assets.tornadoPolygons.features)
    .join("path")
    .attr("class", "tornado-poly poly")
    .attr("d", path);
  layer.selectAll("path.path")
    .data(assets.tornadoPath.features)
    .join("path")
    .attr("class", "tornado-path path")
    .attr("d", path);
}

function backgroundColoringTooltip(d) {
  const p = d.properties;
  const daaConfig = daaBackgroundConfig();
  if (daaConfig) {
    return `
      <strong>${p.name || `Census Block Group ${p.geoid}`}</strong><br>
      ${daaConfig.title}: ${daaConfig.formatter(p[daaConfig.property])}<br>
      SLPS enrollment 2025-26: ${formatNumber(p.slps_enrollment_2025_26_total)}<br>
      SLPS enrollment 2024-25: ${formatNumber(p.slps_enrollment_2024_25_total)}<br>
      Change: ${formatNumber(p.slps_enrollment_change_total)}<br>
      Elementary / Middle / High: ${formatNumber(p.slps_enrollment_2025_26_elementary)} / ${formatNumber(p.slps_enrollment_2025_26_middle)} / ${formatNumber(p.slps_enrollment_2025_26_high)}<br>
      2020 ages 5-17: ${formatNumber(p.census_2020_ages_5_17)}
    `;
  }
  const source = acsSourceNote();
  if (state.backgroundColoring === "race") return `<strong>${p.name}</strong><br>${escapeHtml(source)}<br>Black share: ${formatPct(p.race_black_share)}<br>White share: ${formatPct(p.race_white_share)}`;
  if (state.backgroundColoring === "poverty") return `<strong>${p.name}</strong><br>${escapeHtml(source)}<br>Poverty rate: ${formatPct(p.poverty_rate)}`;
  if (state.backgroundColoring === "schoolAgeTotal") return `<strong>${p.name}</strong><br>${escapeHtml(source)}<br>School-age children: ${formatNumber(p.school_age_5_17)}`;
  if (state.backgroundColoring === "whiteSchoolAgeChildren") {
    return `<strong>${p.name}</strong><br>${escapeHtml(source)}<br>White, non-Hispanic ages 5-17: ${formatNumber(p.school_age_white_non_hispanic_5_17)}<br>Share of school-age residents: ${formatPct(p.school_age_white_non_hispanic_5_17_share)}`;
  }
  if (state.backgroundColoring === "blackSchoolAgeChildren") {
    return `<strong>${p.name}</strong><br>${escapeHtml(source)}<br>Black alone ages 5-17: ${formatNumber(p.school_age_black_alone_5_17)}<br>Share of school-age residents: ${formatPct(p.school_age_black_alone_5_17_share)}`;
  }
  if (state.backgroundColoring === "publicSchoolEnrollment") {
    return `<strong>${p.name}</strong><br>${escapeHtml(source)}<br>Residents ages 5-17 in public school: ${formatNumber(p.public_school_enrolled_5_17)}<br>Share of school-age residents: ${formatPct(p.public_school_enrolled_5_17_per_school_age)}`;
  }
  if (state.backgroundColoring === "privateSchoolEnrollment") {
    return `<strong>${p.name}</strong><br>${escapeHtml(source)}<br>Residents ages 5-17 in private school: ${formatNumber(p.private_school_enrolled_5_17)}<br>Share of school-age residents: ${formatPct(p.private_school_enrolled_5_17_per_school_age)}`;
  }
  return `<strong>${p.name}</strong><br>Tornado damage area: ${formatPct(p.tornado_damage_area_share)}<br>Damage points: ${formatNumber(p.tornado_damage_point_count)}`;
}

function backgroundWaffleTooltip(d) {
  const p = d.properties;
  if (state.backgroundWaffle === "schoolAgeRace") {
    const { total, rows } = schoolAgeRaceRows(p);
    return `<strong>${p.name}</strong><br>${escapeHtml(acsSourceNote())}<br>School-age residents: ${formatNumber(total)}${rows.map((row) => `<br>${row.key}: ${formatNumber(row.value)} (${formatPct(row.share)})`).join("")}`;
  }
  if (state.backgroundWaffle === "publicPrivateSchoolEnrollment") {
    const { total, rows } = publicPrivateEnrollmentRows(p);
    return `<strong>${p.name}</strong><br>${escapeHtml(acsSourceNote())}<br>Residents ages 5-17 enrolled in school: ${formatNumber(total)}${rows.map((row) => `<br>${row.key}: ${formatNumber(row.value)}`).join("")}`;
  }
  return `<strong>${p.name}</strong>`;
}

function renderRegions() {
  const layer = root.select(".region-layer");
  layer.selectAll("*").remove();
  if (!state.showRegions || state.schoolLevel === "all" || state.schoolLevel === "none") return;
  layer.selectAll("path")
    .data(assets.regions[state.schoolLevel].features)
    .join("path")
    .attr("class", "region")
    .attr("d", path)
    .on("mousemove", (event, d) => showTooltip(event, `<strong>${d.properties.school}</strong><br>${d.properties.level} assignment region`))
    .on("mouseleave", hideTooltip);
}

function renderLabels() {
  const layer = root.select(".label-layer");
  layer.selectAll("*").remove();
  if (!state.showNeighborhoods) return;
  const minLabelArea = width < 600 ? 80 : 85;
  const labelFeatures = assets.neighborhoods.features
    .map((feature) => ({ feature, area: path.area(feature) }))
    .filter((d) => d.area >= minLabelArea)
    .sort((a, b) => d3.descending(a.area, b.area))
    .slice(0, width < 600 ? 16 : 58)
    .map((d) => d.feature);
  layer.selectAll("text")
    .data(labelFeatures)
    .join("text")
    .attr("class", "label")
    .attr("x", (d) => path.centroid(d)[0])
    .attr("y", (d) => path.centroid(d)[1])
    .attr("text-anchor", "middle")
    .text((d) => d.properties.neighborhood);
}

function renderSchools() {
  const layer = root.select(".school-layer");
  const groups = layer.selectAll("g.school-glyph")
    .data(visibleSchools(), schoolKey)
    .join((enter) => {
      const g = enter.append("g")
        .attr("class", "school-glyph")
        .attr("tabindex", 0)
        .attr("role", "button")
        .on("mousemove", (event, d) => showTooltip(event, schoolTooltip(d)))
        .on("mouseleave", hideTooltip)
        .on("click", (_, d) => selectSchool(d))
        .on("keydown", (event, d) => {
          if (event.key === "Enter" || event.key === " ") {
            event.preventDefault();
            selectSchool(d);
          }
        });
      g.append("circle").attr("class", "school-outline");
      return g;
    });

  positionSchools();
  groups
    .classed("is-charter", (d) => d.sector === "charter")
    .classed("is-selected", (d) => schoolKey(d) === state.selectedSchoolKey)
    .attr("opacity", (d) => isClosedUnderSelectedPlan(d) ? 0.5 : 1);
  groups.filter((d) => schoolKey(d) === state.selectedSchoolKey).raise();
  groups.select(".school-outline")
    .attr("r", (d) => state.schoolMode === "pto" ? 13 : 12)
    .attr("stroke", (d) => d.sector === "charter" ? colors.sectors.charter : (colors.groups[d.display_group] || colors.groups.specialized));

  groups.each(function(d) {
    const g = d3.select(this);
    g.selectAll(".waffle-square,.pto-marker,.pto-status-text,.enrollment-growth-bar,.enrollment-growth-unavailable,.enrollment-growth-unavailable-slash,.closure-slash").remove();
    if (state.schoolMode === "pto") {
      renderPtoStatus(g, d);
    } else if (state.schoolMode === "enrollmentGrowth") {
      renderEnrollmentGrowthBars(g, d);
    } else {
      renderWaffle(g, d);
    }
    renderClosureSlash(g, d);
  });
}

function renderPrivateSchools() {
  const layer = root.select(".private-school-layer");
  const schools = state.showPrivateSchools ? visiblePrivateSchools() : [];
  const groups = layer.selectAll("g.private-school-glyph")
    .data(schools, (d) => d.key)
    .join((enter) => {
      const g = enter.append("g")
        .attr("class", "private-school-glyph")
        .attr("tabindex", 0)
        .attr("role", "button")
        .on("mousemove", (event, d) => showTooltip(event, schoolTooltip(d)))
        .on("mouseleave", hideTooltip)
        .on("click", (_, d) => selectPrivateSchool(d))
        .on("keydown", (event, d) => {
          if (event.key === "Enter" || event.key === " ") {
            event.preventDefault();
            selectPrivateSchool(d);
          }
        });
      g.append("rect").attr("class", "private-school-halo");
      return g;
  });
  groups.classed("is-selected", (d) => d.key === state.selectedPrivateSchoolKey);
  groups.filter((d) => d.key === state.selectedPrivateSchoolKey).raise();
  groups.each(function(d) {
    renderPrivateSchoolGlyph(d3.select(this), d);
  });
  positionPrivateSchools();
}

function sumSchoolRows(schools, rows) {
  return rows.map((row) => ({
    ...row,
    value: d3.sum(schools, (school) => school[row.field] || 0),
  }));
}

function summaryAttendanceRows(schools) {
  return sumSchoolRows(schoolsWithMetric(schools, "attendance"), [
    { key: "90%+ attendance", field: "attendance_90plus_students", color: colors.attendance[0] },
    { key: "Enrolled below cutoff", field: "attendance_below_90_students", color: colors.attendance[1] },
  ]).concat(sumSchoolRows(schoolsWithMetric(schools, "capacity"), [
    { key: "Empty capacity", field: "empty_capacity_seats", color: colors.attendance[2] },
  ]));
}

function summaryEnrollmentRows(schools) {
  return summaryEnrollmentRowsWithPrivate(schools, []);
}

function summaryEnrollmentRowsWithPrivate(schools, privateSchools = []) {
  return [
    { key: "SLPS enrollment", value: d3.sum(schools.filter((school) => school.sector !== "charter"), (school) => school.enrollment_2025 || 0), color: colors.sectors.slps },
    { key: "Charter campus enrollment", value: d3.sum(schools.filter((school) => school.sector === "charter"), (school) => school.enrollment_2025 || school.campus_enrollment || 0), color: colors.sectors.charter },
    { key: "Private campus enrollment", value: d3.sum(privateSchools.filter((school) => school.availability?.enrollment), (school) => school.enrollment || 0), color: colors.publicPrivateEnrollment.private },
  ].filter((row) => row.value > 0);
}

function summaryEnrollmentBlock(schools, privateSchools = []) {
  const rows = summaryEnrollmentRowsWithPrivate(schools, privateSchools);
  if (!rows.length) return "";
  const total = d3.sum(rows, (row) => row.value || 0);
  return selectionChartBlock("Campus Enrollment By Sector", selectionWaffleHtml(rows), selectionDetailRows(rows, total));
}

function summaryAttendanceBlock(schools) {
  const slpsOnly = slpsSchools(schools);
  const rows = summaryAttendanceRows(slpsOnly);
  const total = d3.sum(rows, (row) => row.value || 0);
  if (!total) return "";
  const attendanceCount = schoolsWithMetric(slpsOnly, "attendance").length;
  const capacityCount = schoolsWithMetric(slpsOnly, "capacity").length;
  const details = `
    <div class="selection-detail-row"><span>SLPS attendance records</span><strong>${formatNumber(attendanceCount)} of ${formatNumber(slpsOnly.length)}</strong></div>
    <div class="selection-detail-row"><span>SLPS capacity records</span><strong>${formatNumber(capacityCount)} of ${formatNumber(slpsOnly.length)}</strong></div>
    ${selectionDetailRows(rows, total)}
    <p class="selection-note">Attendance and building capacity are SLPS-only in the current source contract.</p>
  `;
  return selectionChartBlock("SLPS Attendance / Capacity", selectionWaffleHtml(rows), details);
}

function summaryPovertyRows(schools) {
  return sumSchoolRows(schoolsWithMetric(schools, "direct_certification"), [
    { key: "Direct-certified", field: "direct_cert_count", color: colors.poverty[0], description: deepPovertyTooltip },
    { key: "Not direct-certified", field: "not_direct_cert_count", color: colors.poverty[1], description: deepPovertyTooltip },
  ]);
}

function summaryPovertyBlock(schools) {
  const rows = summaryPovertyRows(schools);
  const total = d3.sum(rows, (row) => row.value || 0);
  if (!total) return "";
  const details = `
    <div class="selection-detail-row"><span>Direct-cert records</span><strong>${formatNumber(schoolsWithMetric(schools, "direct_certification").length)} of ${formatNumber(schools.length)}</strong></div>
    ${selectionDetailRows(rows, total)}
  `;
  return selectionChartBlock("Deep Poverty", selectionWaffleHtml(rows), details);
}

function summaryRaceRows(schools) {
  return sumSchoolRows(schoolsWithMetric(schools, "race"), [
    { key: "Black", field: "black_count", color: colors.race[0] },
    { key: "White", field: "white_count", color: colors.race[1] },
    { key: "Other", field: "other_count", color: colors.race[2] },
  ]);
}

function summaryRaceRowsWithPrivate(schools, privateSchools = []) {
  const publicRows = summaryRaceRows(schools);
  const privateRows = sumRowsByKey(privateSchools.map((school) => ({ rows: privateRaceGlyphRows(school) })), [
    { key: "Black", color: colors.race[0] },
    { key: "White", color: colors.race[1] },
    { key: "Other", color: colors.race[2] },
  ]);
  return publicRows.map((row) => ({
    ...row,
    value: (row.value || 0) + (privateRows.find((privateRow) => privateRow.key === row.key)?.value || 0),
  }));
}

function summaryRaceBlock(schools, privateSchools = []) {
  const rows = summaryRaceRowsWithPrivate(schools, privateSchools);
  const total = d3.sum(rows, (row) => row.value || 0);
  if (!total) return "";
  const publicRecordCount = schoolsWithMetric(schools, "race").length;
  const privateRecordCount = privateSchools.filter((school) => privateRaceGlyphRows(school).length).length;
  const privateRecordDetail = privateSchools.length
    ? `<div class="selection-detail-row"><span>Private PSS race records</span><strong>${formatNumber(privateRecordCount)} of ${formatNumber(privateSchools.length)}</strong></div>`
    : "";
  const details = `
    <div class="selection-detail-row"><span>Public race records</span><strong>${formatNumber(publicRecordCount)} of ${formatNumber(schools.length)}</strong></div>
    ${privateRecordDetail}
    ${selectionDetailRows(rows, total)}
  `;
  return selectionChartBlock("Race", selectionWaffleHtml(rows), details);
}

function summaryEnrollmentGrowthBlock(schools, privateSchools = []) {
  const includedSchools = [...schools, ...privateSchools];
  const counts = d3.rollup(includedSchools, (items) => items.length, (school) => enrollmentGrowthData(school)?.direction || "unavailable");
  const rows = [
    { key: "Growing", value: counts.get("growing") || 0, color: colors.enrollmentGrowth.growing },
    { key: "Steady", value: counts.get("steady") || 0, color: colors.enrollmentGrowth.steady },
    { key: "Shrinking", value: counts.get("shrinking") || 0, color: colors.enrollmentGrowth.shrinking },
    { key: "Unavailable", value: counts.get("unavailable") || 0, color: colors.enrollmentGrowth.unavailable },
  ];
  const total = d3.sum(rows, (row) => row.value || 0);
  if (!total) return "";
  const available = includedSchools.filter((school) => enrollmentGrowthData(school)).length;
  const publicAvailable = schools.filter((school) => enrollmentGrowthData(school)).length;
  const privateAvailable = privateSchools.filter((school) => enrollmentGrowthData(school)).length;
  const details = `
    <div class="selection-detail-row"><span>Enrollment histories</span><strong>${formatNumber(available)} of ${formatNumber(includedSchools.length)}</strong></div>
    ${privateSchools.length ? `<div class="selection-detail-row"><span>Public / private histories</span><strong>${formatNumber(publicAvailable)} / ${formatNumber(privateAvailable)}</strong></div>` : ""}
    ${selectionDetailRows(rows, total)}
    <p class="selection-note">Trend color uses least-squares slope over each school's latest available annual enrollment records, up to 10 years; at least two annual records are required for a slope-based trend.</p>
  `;
  return selectionChartBlock("Enrollment Growth", selectionWaffleHtml(rows, 1), details);
}

function summaryPtoRows(schools) {
  const counts = d3.rollup(schoolsWithMetric(schools, "pto"), (items) => items.length, (school) => school.pto_status || "unknown");
  return Object.entries(ptoStatuses).map(([status, meta]) => ({
    key: meta.label,
    value: counts.get(status) || 0,
    color: colors.pto[status] || colors.pto.unknown,
  }));
}

function sumRowsByKey(items, order) {
  return order.map((template) => ({
    ...template,
    value: d3.sum(items, (item) => {
      const row = item.rows.find((candidate) => candidate.key === template.key);
      return row?.value || 0;
    }),
  }));
}

function summaryAssessmentPerformance(schools) {
  const markers = schools.map(assessmentMarkerPerformance).filter(Boolean);
  const order = [
    { key: "Below Basic", color: colors.assessment.below_basic },
    { key: "Basic", color: colors.assessment.basic },
    { key: "Proficient", color: colors.assessment.proficient },
    { key: "Advanced", color: colors.assessment.advanced },
    { key: "Missing", color: colors.assessment.missing },
  ];
  const rows = sumRowsByKey(markers, order);
  const tested = d3.sum(markers, (marker) => marker.nSize || 0);
  const proficientAdvancedRecords = d3.sum(markers, (marker) => {
    if (!Number.isFinite(marker.proficientOrAdvanced) || !Number.isFinite(marker.nSize)) return 0;
    return marker.nSize * marker.proficientOrAdvanced / 100;
  });
  return {
    rows,
    tested,
    schoolCount: markers.length,
    proficientAdvancedRate: tested > 0 ? proficientAdvancedRecords / tested : null,
  };
}

function summaryAssessmentIep(schools) {
  const markers = schools.map((school) => assessmentMarkerIep(school)).filter(Boolean);
  const rows = sumRowsByKey(markers, [
    { key: "Students with an IEP", color: colors.assessment.iep },
    { key: "Students without an IEP", color: colors.assessment.non_iep },
  ]);
  return {
    rows,
    tested: d3.sum(markers, (marker) => marker.nSize || 0),
    schoolCount: markers.length,
    derivedCellCount: d3.sum(schools, (school) => school.assessment_2025?.iep_composition_aggregate?.derived_cell_count || 0),
    cellCount: d3.sum(schools, (school) => school.assessment_2025?.iep_composition_aggregate?.cell_count || 0),
  };
}

function summaryMetricBlock(title, rows, unit = 20) {
  const total = d3.sum(rows, (row) => row.value || 0);
  return selectionChartBlock(title, selectionWaffleHtml(rows, unit), selectionDetailRows(rows, total));
}

function summaryAssessmentPerformanceBlock(schools) {
  const summary = summaryAssessmentPerformance(schools);
  const total = d3.sum(summary.rows, (row) => row.value || 0);
  if (!total) return "";
  const details = `
    <div class="selection-detail-row"><span>Schools with usable MAP</span><strong>${formatNumber(summary.schoolCount)} of ${formatNumber(schools.length)}</strong></div>
    <div class="selection-detail-row"><span>Tested records</span><strong>${formatNumber(summary.tested)}</strong></div>
    <div class="selection-detail-row"><span>Proficient or advanced</span><strong>${formatPct(summary.proficientAdvancedRate)}</strong></div>
    ${selectionDetailRows(summary.rows, total)}
    <p class="selection-note">Aggregates each school's selected usable MAP content/grade cell; waffle squares are approximate tested-record counts.</p>
  `;
  return selectionChartBlock("MAP Performance", selectionWaffleHtml(summary.rows, 20), details);
}

function summaryAssessmentIepBlock(schools) {
  const summary = summaryAssessmentIep(schools);
  const total = d3.sum(summary.rows, (row) => row.value || 0);
  if (!total) return "";
  const details = `
    <div class="selection-detail-row"><span>Schools with IEP MAP data</span><strong>${formatNumber(summary.schoolCount)} of ${formatNumber(schools.length)}</strong></div>
    <div class="selection-detail-row"><span>Included cells</span><strong>${formatNumber(summary.cellCount)}</strong></div>
    ${summary.derivedCellCount ? `<div class="selection-detail-row"><span>Derived cells</span><strong>${formatNumber(summary.derivedCellCount)}</strong></div>` : ""}
    <div class="selection-detail-row"><span>Tested records</span><strong>${formatNumber(summary.tested)}</strong></div>
    ${selectionDetailRows(summary.rows, total)}
    <p class="selection-note">Aggregated MAP-tested records across available content/grade cells, not unique students or full-school enrollment.</p>
  `;
  return selectionChartBlock("MAP IEP Composition", selectionWaffleHtml(summary.rows, 20), details);
}

function sumFeatureProperties(features, fields) {
  return Object.fromEntries(fields.map((field) => [
    field,
    d3.sum(features, (feature) => feature.properties?.[field] || 0),
  ]));
}

function formatAcres(squareMeters) {
  if (!Number.isFinite(squareMeters)) return "n/a";
  return `${formatNumber(squareMeters * 0.000247105)} acres`;
}

function backgroundSummaryHtml() {
  const features = assets.demographics.features || [];
  const zips = assets.zips.features || [];
  const counts = sumFeatureProperties(features, [
    "total_population",
    "race_total",
    "race_black_non_hispanic",
    "race_white_non_hispanic",
    "race_other",
    "poverty_universe",
    "poverty_below",
    "school_age_5_17",
    "school_age_black_alone_5_17",
    "school_age_white_non_hispanic_5_17",
    "school_age_other_or_unclassified_5_17",
    "public_school_enrolled_5_17",
    "private_school_enrolled_5_17",
    "tornado_damage_area_sq_m",
    "tornado_damage_point_count",
    "tornado_ef2_plus_point_count",
  ]);
  const populationTotal = counts.race_total || counts.total_population || 0;
  const schoolAgeTotal = counts.school_age_5_17 || 0;
  const enrollmentTotal = counts.public_school_enrolled_5_17 + counts.private_school_enrolled_5_17;
  const povertyAtOrAbove = Math.max(0, counts.poverty_universe - counts.poverty_below);
  const zhviValues = zips.map((feature) => feature.properties?.zhvi_typical_home_value).filter(Number.isFinite);
  const zhviMonth = zhviMonthLabel();
  const populationRaceRows = [
    { key: "Black, non-Hispanic", value: counts.race_black_non_hispanic, color: colors.race[0] },
    { key: "White, non-Hispanic", value: counts.race_white_non_hispanic, color: colors.race[1] },
    { key: "Other", value: counts.race_other, color: colors.race[2] },
  ];
  const schoolAgeRows = [
    { key: "Black alone", value: displaySafeCount(counts.school_age_black_alone_5_17), color: colors.schoolAgeRace.black },
    { key: "White, non-Hispanic", value: displaySafeCount(counts.school_age_white_non_hispanic_5_17), color: colors.schoolAgeRace.white },
    { key: "Other / residual", value: displaySafeCount(counts.school_age_other_or_unclassified_5_17), color: colors.schoolAgeRace.other },
  ];
  const enrollmentRows = [
    { key: "Public school", value: counts.public_school_enrolled_5_17, color: colors.publicPrivateEnrollment.public },
    { key: "Private school", value: counts.private_school_enrolled_5_17, color: colors.publicPrivateEnrollment.private },
  ];
  const povertyRows = [
    { key: "Below poverty threshold", value: counts.poverty_below, color: "#b42318" },
    { key: "At or above threshold", value: povertyAtOrAbove, color: "#8fb8a3" },
  ];

  return `
    <section class="summary-group">
      <h3>Background Summary</h3>
      <div class="summary-kpis">
        <div><span>Total population</span><strong>${formatNumber(counts.total_population)}</strong></div>
        <div><span>School-age residents</span><strong>${formatNumber(schoolAgeTotal)}</strong></div>
        <div><span>Ages 5-17 share</span><strong>${formatPct(schoolAgeTotal / counts.total_population)}</strong></div>
        <div><span>Public school enrolled</span><strong>${formatPct(counts.public_school_enrolled_5_17 / enrollmentTotal)}</strong></div>
        <div><span>Poverty rate</span><strong>${formatPct(counts.poverty_below / counts.poverty_universe)}</strong></div>
        <div><span>Median ZIP ZHVI${zhviMonth ? ` ${escapeHtml(zhviMonth)}` : ""}</span><strong>${formatMoney(d3.median(zhviValues))}</strong></div>
      </div>
      ${selectionChartBlock(`${acsYear()} ACS Population Race`, selectionWaffleHtml(populationRaceRows, 1000), selectionDetailRows(populationRaceRows, populationTotal))}
      ${selectionChartBlock(`${acsYear()} ACS School-Age Race`, selectionWaffleHtml(schoolAgeRows, 100), selectionDetailRows(schoolAgeRows, schoolAgeTotal))}
      ${selectionChartBlock(`${acsYear()} ACS Public vs. Private Enrollment Ages 5-17`, selectionWaffleHtml(enrollmentRows, 200), selectionDetailRows(enrollmentRows, enrollmentTotal))}
      ${selectionChartBlock(`${acsYear()} ACS Poverty Indicators`, selectionWaffleHtml(povertyRows, 1000), selectionDetailRows(povertyRows, counts.poverty_universe))}
      <section class="selection-chart-block">
        <h3>May 16, 2025 Tornado Damage Context</h3>
        <div class="selection-plan-list">
          <div class="selection-detail-row"><span>Damage observations</span><strong>${formatNumber(counts.tornado_damage_point_count)}</strong></div>
          <div class="selection-detail-row"><span>EF2+ observations</span><strong>${formatNumber(counts.tornado_ef2_plus_point_count)}</strong></div>
          <div class="selection-detail-row"><span>Damage polygon area</span><strong>${formatAcres(counts.tornado_damage_area_sq_m)}</strong></div>
        </div>
      </section>
      <p class="selection-note">Background totals are ${escapeHtml(acsDatasetLabel())} ${escapeHtml(acsGeographyLabel().toLowerCase())} estimates clipped to St. Louis City. Public/private enrollment is residence-based and is not SLPS-operated enrollment.</p>
    </section>
  `;
}

function summaryMetricEnabled(metrics, metric) {
  return !metrics || metrics.has(metric);
}

function summaryKpiRows(schools, privateSchools = [], metrics = null) {
  const capacitySchools = schoolsWithMetric(schools, "capacity");
  const slpsCount = schools.filter((school) => school.sector !== "charter").length;
  const charterCount = schools.filter((school) => school.sector === "charter").length;
  const privateCount = privateSchools.length;
  const rows = [
    ["Schools", formatNumber(schools.length + privateCount)],
    ["SLPS / Charter / Private", `${formatNumber(slpsCount)} / ${formatNumber(charterCount)} / ${formatNumber(privateCount)}`],
    ["Campus enrollment", formatNumber(d3.sum(schoolsWithMetric(schools, "enrollment"), (school) => school.enrollment_2025 || school.campus_enrollment || 0) + d3.sum(privateSchools.filter((school) => school.availability?.enrollment), (school) => school.enrollment || 0))],
  ];
  if (summaryMetricEnabled(metrics, "attendance")) {
    rows.push(
      ["SLPS attendance denominator", formatNumber(d3.sum(schoolsWithMetric(slpsSchools(schools), "attendance"), (school) => school.par_students_total || 0))],
      ["SLPS capacity", formatNumber(d3.sum(capacitySchools, (school) => school.official_building_capacity || 0))],
      ["Empty seats", formatNumber(d3.sum(capacitySchools, (school) => school.empty_capacity_seats || 0))],
      ["SLPS capacity records", `${formatNumber(capacitySchools.length)} of ${formatNumber(slpsCount)}`],
    );
  }
  return `
    <div class="summary-kpis">
      ${rows.map(([label, value]) => `<div><span>${escapeHtml(label)}</span><strong>${escapeHtml(value)}</strong></div>`).join("")}
    </div>
  `;
}

function summaryGroupHtml(label, schools, privateSchools = [], options = {}) {
  const metrics = options.metrics || null;
  return `
    <section class="summary-group">
      <h3>${escapeHtml(label)}</h3>
      ${summaryKpiRows(schools, privateSchools, metrics)}
      ${summaryMetricEnabled(metrics, "enrollment") ? summaryEnrollmentBlock(schools, privateSchools) : ""}
      ${summaryMetricEnabled(metrics, "attendance") ? `${summaryAttendanceBlock(schools)}<br/>${descriptions.attendance}<br/>` : ""}
      ${summaryMetricEnabled(metrics, "poverty") ? `${summaryPovertyBlock(schools)}<br/>${descriptions.deep_poverty}<br/>` : ""}
      ${summaryMetricEnabled(metrics, "race") ? summaryRaceBlock(schools, privateSchools) : ""}
      ${summaryMetricEnabled(metrics, "enrollmentGrowth") ? summaryEnrollmentGrowthBlock(schools, privateSchools) : ""}
      ${summaryMetricEnabled(metrics, "assessmentPerformance") ? summaryAssessmentPerformanceBlock(schools) : ""}
      ${summaryMetricEnabled(metrics, "assessmentIep") ? summaryAssessmentIepBlock(schools) : ""}
      ${summaryMetricEnabled(metrics, "pto") ? `${summaryMetricBlock("PTO Activity", summaryPtoRows(schools), 1)}<br/>${descriptions.pto}<br/>` : ""}
    </section>
  `;
}

function groupHasSummaryMetric(group, metric) {
  const schools = group.schools || [];
  const privateSchools = group.privateSchools || [];
  if (metric === "enrollment") {
    return summaryEnrollmentRowsWithPrivate(schools, privateSchools).some((row) => row.value > 0);
  }
  if (metric === "attendance") {
    return d3.sum(summaryAttendanceRows(slpsSchools(schools)), (row) => row.value || 0) > 0;
  }
  if (metric === "poverty") {
    return d3.sum(summaryPovertyRows(schools), (row) => row.value || 0) > 0;
  }
  if (metric === "race") {
    return d3.sum(summaryRaceRowsWithPrivate(schools, privateSchools), (row) => row.value || 0) > 0;
  }
  if (metric === "enrollmentGrowth") {
    return [...schools, ...privateSchools].some((school) => Boolean(enrollmentGrowthData(school)));
  }
  if (metric === "assessmentPerformance") {
    return d3.sum(summaryAssessmentPerformance(schools).rows, (row) => row.value || 0) > 0;
  }
  if (metric === "assessmentIep") {
    return d3.sum(summaryAssessmentIep(schools).rows, (row) => row.value || 0) > 0;
  }
  if (metric === "pto") {
    return d3.sum(summaryPtoRows(schools), (row) => row.value || 0) > 0;
  }
  return false;
}

function sharedComparisonMetrics(groups) {
  const metricOrder = ["enrollment", "attendance", "poverty", "race", "enrollmentGrowth", "assessmentPerformance", "assessmentIep", "pto"];
  return new Set(metricOrder.filter((metric) => groups.every((group) => groupHasSummaryMetric(group, metric))));
}

function comparisonPlanButtonsHtml() {
  return `
    <div class="segmented compact comparison-plan-picker" role="radiogroup" aria-label="Select closure plan">
      ${closurePlans.filter((plan) => plan.id !== "none").map((plan) => `
        <button type="button" data-closure-plan="${escapeHtml(plan.id)}">${escapeHtml(plan.label)}</button>
      `).join("")}
    </div>
  `;
}

function closurePlanEmptyPanelHtml() {
  return `
    <section class="comparison-panel">
      <section class="summary-group">
        <div class="selection-empty-chart">
          <div>Select a closure plan to compare "open" vs "closed" schools.</div>
          ${comparisonPlanButtonsHtml()}
        </div>
      </section>
    </section>
  `;
}

function bindComparisonPlanButtons() {
  selection.selectAll("[data-closure-plan]").on("click", function() {
    const planId = this.getAttribute("data-closure-plan");
    if (!planId) return;
    state.closurePlan = planId;
    updateControls();
    renderSchools();
    renderLegend();
    renderMetadata();
    refreshSelection();
  });
}

function comparisonPanelHtml(group, metrics) {
  const publicCount = group.schools?.length || 0;
  const privateCount = group.privateSchools?.length || 0;
  if (!publicCount && !privateCount) {
    return `
      <section class="comparison-panel">
        <h3>${escapeHtml(group.label)}</h3>
        <div class="selection-empty-chart">No schools shown</div>
      </section>
    `;
  }
  return `
    <section class="comparison-panel">
      ${summaryGroupHtml(group.label, group.schools || [], group.privateSchools || [], { metrics })}
    </section>
  `;
}

function sidebarComparisonGroups(schools, privateSchools) {
  if (state.sidebarComparison === "closure") {
    if (state.closurePlan === "none") return null;
    const slps = slpsSchools(schools);
    return [
      { label: "Schools staying open", schools: slps.filter((school) => !isClosedUnderSelectedPlan(school)) },
      { label: "Schools closing", schools: slps.filter(isClosedUnderSelectedPlan) },
    ];
  }
  if (state.sidebarComparison === "slpsCharter") {
    return [
      { label: "SLPS", schools: schools.filter((school) => school.sector !== "charter") },
      { label: "Charter", schools: schools.filter((school) => school.sector === "charter") },
    ];
  }
  if (state.sidebarComparison === "slpsPrivate") {
    return [
      { label: "SLPS", schools: schools.filter((school) => school.sector !== "charter") },
      { label: "Private", schools: [], privateSchools },
    ];
  }
  if (state.sidebarComparison === "charterPrivate") {
    return [
      { label: "Charter", schools: schools.filter((school) => school.sector === "charter") },
      { label: "Private", schools: [], privateSchools },
    ];
  }
  return null;
}

function renderSidebarComparison() {
  if (!assets) return;
  const schools = visibleSchools();
  const privateSchools = visibleSummaryPrivateSchools();
  const selectedComparison = sidebarComparisons.find((comparison) => comparison.id === state.sidebarComparison);
  const levelLabel = state.schoolLevel === "all" ? "all school levels" : state.schoolLevel === "none" ? "school layers hidden" : `${state.schoolLevel} schools`;
  const selectedPlan = closurePlans.find((plan) => plan.id === state.closurePlan);
  const groups = sidebarComparisonGroups(schools, privateSchools);

  if (!groups) {
    selection.html(`
      <strong>${escapeHtml(selectedComparison?.label || "Comparison")}</strong>
      <div class="selection-subtitle">Choose Plan A, B, or C to compare schools staying open with schools closing.</div>
      ${closurePlanEmptyPanelHtml()}
      ${backgroundSummaryHtml()}
    `);
    bindComparisonPlanButtons();
    return;
  }

  const metrics = sharedComparisonMetrics(groups);
  selection.html(`
    <strong>${escapeHtml(selectedComparison?.label || "Comparison")}</strong>
    <div class="selection-subtitle">${formatNumber(schools.length + privateSchools.length)} rendered schools, ${escapeHtml(levelLabel)}${state.sidebarComparison === "closure" && selectedPlan ? `, ${escapeHtml(selectedPlan.label)}` : ""}</div>
    <div class="comparison-grid">
      ${groups.map((group) => comparisonPanelHtml(group, metrics)).join("")}
    </div>
    ${backgroundSummaryHtml()}
  `);
}

function renderSummarySelection() {
  if (!assets) return;
  const schools = visibleSchools();
  const privateSchools = visibleSummaryPrivateSchools();
  if (state.schoolLevel === "none") {
    selection.html(`
      <strong>Summary</strong>
      <div class="selection-subtitle">School layers hidden.</div>
      ${backgroundSummaryHtml()}
    `);
    return;
  }
  const levelLabel = state.schoolLevel === "all" ? "all school levels" : `${state.schoolLevel} schools`;
  const selectedPlan = closurePlans.find((plan) => plan.id === state.closurePlan);
  const groups = state.closurePlan === "none"
    ? [{ label: "Shown schools", schools, privateSchools }]
    : [
        { label: "SLPS remain open", schools: slpsSchools(schools).filter((school) => !isClosedUnderSelectedPlan(school)) },
        { label: "SLPS closed or repurposed", schools: slpsSchools(schools).filter(isClosedUnderSelectedPlan) },
        { label: "Charter schools", schools: schools.filter((school) => school.sector === "charter") },
        { label: "Private schools", schools: [], privateSchools },
      ];

  selection.html(`
    <strong>Summary</strong>
    <div class="selection-subtitle">${formatNumber(schools.length + privateSchools.length)} rendered schools, ${escapeHtml(levelLabel)}${selectedPlan && state.closurePlan !== "none" ? `, ${escapeHtml(selectedPlan.label)}` : ""}</div>
    ${summarySchoolSearchHtml()}
    ${groups.filter((group) => group.schools.length || group.privateSchools?.length).map((group) => summaryGroupHtml(group.label, group.schools, group.privateSchools || [])).join("")}
    ${backgroundSummaryHtml()}
  `);
  bindSummarySchoolSearch();
}

function renderClosureSlash(g, school) {
  if (!isClosedUnderSelectedPlan(school)) return;
  const radius = state.schoolMode === "pto" ? 16 : 15;
  g.append("line")
    .attr("class", "closure-slash")
    .attr("x1", -radius)
    .attr("y1", radius)
    .attr("x2", radius)
    .attr("y2", -radius);
}

function positionSchools() {
  if (!projection || !root) return;
  const scale = state.schoolGlyphScale / state.transform.k;
  root.selectAll(".school-glyph")
    .attr("transform", (d) => `translate(${projection([d.lon, d.lat])}) scale(${scale * (schoolKey(d) === state.selectedSchoolKey ? 2 : 1)})`);
}

function positionPrivateSchools() {
  if (!projection || !root) return;
  const scale = state.schoolGlyphScale / state.transform.k;
  root.selectAll(".private-school-glyph")
    .attr("transform", (d) => `translate(${projection([d.lon, d.lat])}) scale(${scale * (d.key === state.selectedPrivateSchoolKey ? 2 : 1)})`);
}

function renderWaffle(g, school) {
  const marker = assessmentMarkerData(school);
  const rows = marker?.rows || schoolCategories(school);
  const squares = roundedSquares(rows, marker?.unit || 20);
  const size = 2.9;
  const gap = 0.45;
  const columns = Math.max(3, Math.ceil(Math.sqrt(Math.max(1, squares.length))));
  const rowsCount = Math.ceil(squares.length / columns);
  const x0 = -((columns * size + (columns - 1) * gap) / 2);
  const y0 = -((rowsCount * size + (rowsCount - 1) * gap) / 2);
  g.selectAll(".waffle-square")
    .data(squares)
    .join("rect")
    .attr("class", "waffle-square")
    .attr("width", size)
    .attr("height", size)
    .attr("rx", 0.4)
    .attr("x", (_, i) => x0 + (i % columns) * (size + gap))
    .attr("y", (_, i) => y0 + Math.floor(i / columns) * (size + gap))
    .attr("fill", (d) => d.color);
}

function renderEnrollmentGrowthBars(g, school) {
  const data = enrollmentGrowthData(school);
  if (!data) {
    g.append("circle")
      .attr("class", "enrollment-growth-unavailable")
      .attr("r", 7.4);
    g.append("line")
      .attr("class", "enrollment-growth-unavailable-slash")
      .attr("x1", -4.5)
      .attr("y1", 4.5)
      .attr("x2", 4.5)
      .attr("y2", -4.5);
    return;
  }
  const width = 1.45;
  const gap = 0.36;
  const maxHeight = 14;
  const x0 = -((data.history.length * width + (data.history.length - 1) * gap) / 2);
  const fill = enrollmentGrowthColor(school);
  g.selectAll(".enrollment-growth-bar")
    .data(data.history)
    .join("rect")
    .attr("class", "enrollment-growth-bar")
    .attr("width", width)
    .attr("rx", 0.25)
    .attr("x", (_, i) => x0 + i * (width + gap))
    .attr("y", (d) => 7 - Math.max(0.8, (d.enrollment / data.maxEnrollment) * maxHeight))
    .attr("height", (d) => Math.max(0.8, (d.enrollment / data.maxEnrollment) * maxHeight))
    .attr("fill", fill);
}

function renderPtoStatus(g, school) {
  if (!hasMetric(school, "pto")) {
    g.append("circle")
      .attr("class", "pto-marker pto-unavailable")
      .attr("r", 7.4)
      .attr("fill", "#f5f2ed")
      .attr("stroke", colors.sectors.charter)
      .attr("stroke-width", 1.8)
      .attr("stroke-dasharray", "2 2");
    g.append("text")
      .attr("class", "pto-status-text")
      .attr("text-anchor", "middle")
      .attr("dy", "0.34em")
      .text("x");
    return;
  }
  const status = school.pto_status || "unknown";
  const meta = ptoStatusMeta(status);
  const color = colors.pto[status] || colors.pto.unknown;
  g.append("circle")
    .attr("class", `pto-marker pto-${status}`)
    .attr("r", 7.4)
    .attr("fill", color)
    .attr("stroke", status === "unknown" ? "#7d8790" : "#ffffff")
    .attr("stroke-width", status === "unknown" ? 1.4 : 1.8)
    .attr("stroke-dasharray", status === "unknown" ? "2 2" : null);
  g.append("text")
    .attr("class", "pto-status-text")
    .attr("text-anchor", "middle")
    .attr("dy", "0.34em")
    .text(ptoStatusGlyph(status, meta));
}

function selectionWaffleHtml(rows, unit = 20) {
  const squares = roundedSquares(rows, unit);
  if (!squares.length) {
    return `<div class="selection-empty-chart">n/a</div>`;
  }
  const size = 6.3;
  const gap = 1.1;
  const columns = Math.max(5, Math.ceil(Math.sqrt(squares.length)));
  const rowsCount = Math.ceil(squares.length / columns);
  const width = columns * size + (columns - 1) * gap;
  const height = rowsCount * size + (rowsCount - 1) * gap;
  return `
    <svg class="selection-waffle" viewBox="0 0 ${width} ${height}" role="img" aria-label="Waffle chart">
      ${squares.map((square, index) => `
        <rect
          x="${(index % columns) * (size + gap)}"
          y="${Math.floor(index / columns) * (size + gap)}"
          width="${size}"
          height="${size}"
          rx="1.4"
          fill="${escapeHtml(square.color)}"
        ></rect>
      `).join("")}
    </svg>
  `;
}

function selectionPtoHtml(school) {
  const status = school.pto_status || "unknown";
  const meta = ptoStatusMeta(status);
  const color = colors.pto[status] || colors.pto.unknown;
  return `
    <div class="selection-pto-marker" style="--pto-color:${escapeHtml(color)}">
      <span>${escapeHtml(ptoStatusGlyph(status, meta))}</span>
    </div>
  `;
}

function selectionEnrollmentGrowthHtml(school) {
  const data = enrollmentGrowthData(school);
  if (!data) return `<div class="selection-empty-chart">n/a</div>`;
  const width = 160;
  const height = 64;
  const padding = 4;
  const gap = 3;
  const barWidth = (width - padding * 2 - gap * (data.history.length - 1)) / data.history.length;
  const color = enrollmentGrowthColor(school);
  return `
    <svg class="selection-growth-bars" viewBox="0 0 ${width} ${height}" role="img" aria-label="${escapeHtml(enrollmentGrowthWindowLabel(data))}">
      ${data.history.map((point, index) => {
        const barHeight = Math.max(2, (point.enrollment / data.maxEnrollment) * (height - padding * 2));
        return `
          <rect
            x="${padding + index * (barWidth + gap)}"
            y="${height - padding - barHeight}"
            width="${barWidth}"
            height="${barHeight}"
            rx="1.5"
            fill="${escapeHtml(color)}"
          ></rect>
        `;
      }).join("")}
    </svg>
  `;
}

function selectionEnrollmentGrowthDetails(school) {
  const data = enrollmentGrowthData(school);
  if (!data) {
    return `<p class="selection-note">No enrollment history is available for this campus.</p>`;
  }
  const first = data.history[0];
  const latest = data.history[data.history.length - 1];
  const source = school.enrollment_history_source_label || "NCES CCD/EDGE";
  return `
    <div class="selection-detail-row"><span>Range</span><strong>${escapeHtml(enrollmentGrowthRangeLabel(data))}</strong></div>
    <div class="selection-detail-row"><span>First enrollment</span><strong>${formatNumber(first.enrollment)}</strong></div>
    <div class="selection-detail-row"><span>Latest enrollment</span><strong>${formatNumber(latest.enrollment)}</strong></div>
    <div class="selection-detail-row"><span>Displayed max</span><strong>${formatNumber(data.maxEnrollment)}</strong></div>
    <div class="selection-detail-row"><span>Trend</span><strong>${escapeHtml(enrollmentGrowthDirectionLabel(data.direction, data.trendAvailable))}</strong></div>
    <div class="selection-detail-row"><span>Slope</span><strong>${Number.isFinite(data.slope) ? `${d3.format("+,.1f")(data.slope)} / year` : "n/a"}</strong></div>
    <div class="selection-detail-row"><span>Source</span><strong>${escapeHtml(source)}</strong></div>
    <p class="selection-note">Bars use the latest ${formatNumber(data.history.length)} available annual records and are scaled to this school's own maximum enrollment in the displayed window.</p>
  `;
}

function selectionDetailRows(rows, total) {
  return rows.map((row) => `
    <div class="selection-detail-row"${row.description ? ` title="${escapeHtml(row.description)}"` : ""}>
      <span><span class="swatch" style="background:${escapeHtml(row.color)}"></span>${escapeHtml(row.key)}</span>
      <strong>${formatDetailValue(row.value, total)}</strong>
    </div>
  `).join("");
}

function selectionPerformanceDetailRows(rows) {
  return rows.map((row) => `
    <div class="selection-detail-row">
      <span><span class="swatch" style="background:${escapeHtml(row.color)}"></span>${escapeHtml(row.key)}</span>
      <strong>${formatNumber(row.value)} (${formatMapPct(row.pct)})</strong>
    </div>
  `).join("");
}

function areaComparisonPrefix(comparison) {
  return comparison?.scope === "assignment_region" ? "Neighborhood" : "City";
}

function areaComparisonRow(label, value, description) {
  return `
    <div class="selection-detail-row"${description ? ` title="${escapeHtml(description)}"` : ""}>
      <span>${escapeHtml(label)}</span>
      <strong>${formatPct(value)}</strong>
    </div>
  `;
}

function areaComparisonNote(comparison) {
  if (!comparison) return "";
  const note = comparison.scope === "assignment_region"
    ? "Neighborhood demographics estimated from census tract data, aligned with the parcels that are assigned to this school."
    : "City demographics estimated from census tract data.";
  return `
    <p class="selection-note">${escapeHtml(note)}</p>
  `;
}

function selectionPovertyDetailRows(rows, total, comparison) {
  const areaDescription = "ACS resident poverty rate for the selected school's comparison area.";
  const areaPrefix = areaComparisonPrefix(comparison);
  return `
    ${selectionDetailRows(rows, total)}
    ${areaComparisonRow(`${areaPrefix} poverty rate`, comparison?.poverty_rate, areaDescription)}
  `;
}

function selectionRaceDetailRows(rows, total, comparison) {
  const areaDescription = "ACS resident race/ethnicity share for the selected school's comparison area.";
  const areaPrefix = areaComparisonPrefix(comparison);
  const areaRows = [
    [`${areaPrefix} Black`, comparison?.race_black_share],
    [`${areaPrefix} White`, comparison?.race_white_share],
    [`${areaPrefix} Other`, comparison?.race_other_share],
  ];
  return `
    ${selectionDetailRows(rows, total)}
    ${areaRows.map(([label, value]) => areaComparisonRow(label, value, areaDescription)).join("")}
  `;
}

function ptoDetailRows(d, meta) {
  const rows = [`<div class="selection-detail-row"><span>Status</span><strong>${escapeHtml(meta.label)}</strong></div>`];
  if (d.pto_status !== "unknown") {
    rows.push(`<div class="selection-detail-row"><span>Confidence</span><strong>${escapeHtml(d.pto_confidence || "unknown")}</strong></div>`);
    rows.push(`<div class="selection-detail-row"><span>Source</span><strong>${ptoSourceHtml(d)}</strong></div>`);
  }
  return rows.join("");
}

function selectionChartBlock(title, chartHtml, detailHtml) {
  return `
    <section class="selection-chart-block">
      <h3>${escapeHtml(title)}</h3>
      <div class="selection-chart-row">
        <div class="selection-chart">${chartHtml}</div>
        <div class="selection-chart-details">${detailHtml}</div>
      </div>
    </section>
  `;
}

function assessmentPerformanceBlock(assessment) {
  const content = bestAssessmentPerformanceContent(assessment);
  if (!content) return "";
  const grade = content.all_students.default_grade;
  const record = content.all_students.grades[grade];
  const rows = assessmentPerformanceRows(record);
  if (!rows.length || !Number.isFinite(record.proficient_or_advanced_pct)) return "";
  const details = `
    <div class="selection-detail-row"><span>Content / grade</span><strong>${escapeHtml(content.label)} / ${escapeHtml(gradeLabel(grade))}</strong></div>
    <div class="selection-detail-row"><span>Tested students</span><strong>${formatNumber(record.n_size)}</strong></div>
    <div class="selection-detail-row"><span>Proficient or advanced</span><strong>${formatMapPct(record.proficient_or_advanced_pct)}</strong></div>
    ${selectionPerformanceDetailRows(rows)}
  `;
  return selectionChartBlock("Assessment 2025: Performance", selectionWaffleHtml(rows, 20), details);
}

function assessmentIepBlock(assessment) {
  const aggregate = assessment?.iep_composition_aggregate;
  if (aggregate) {
    const rows = [
      { key: "Students with an IEP", value: aggregate.iep_n_size, color: colors.assessment.iep },
      { key: "Students without an IEP", value: aggregate.non_iep_n_size, color: colors.assessment.non_iep },
    ];
    const total = d3.sum(rows, (row) => row.value || 0);
    if (total) {
      const unit = 20;
      const details = `
        <div class="selection-detail-row"><span>Included cells</span><strong>${formatNumber(aggregate.cell_count)}</strong></div>
        ${aggregate.derived_cell_count ? `<div class="selection-detail-row"><span>Derived cells</span><strong>${formatNumber(aggregate.derived_cell_count)}</strong></div>` : ""}
        <div class="selection-detail-row"><span>Tested records</span><strong>${formatNumber(aggregate.all_students_n_size || total)}</strong></div>
        ${selectionDetailRows(rows, total)}
        <p class="selection-note">${aggregate.derived_cell_count ? "Aggregated MAP-tested records across available content/grade cells; some IEP counts are derived from All Students minus Non IEP." : "Aggregated MAP-tested records across available paired content/grade cells."} Not unique students or full-school enrollment.</p>
      `;
      return selectionChartBlock("Assessment 2025: IEP Composition", selectionWaffleHtml(rows, unit), details);
    }
  }
  const content = bestAssessmentContent(assessment, "iep_composition");
  if (!content) return "";
  const grade = content.iep_composition.default_grade;
  const record = content.iep_composition.grades[grade];
  const rows = [
    { key: "Students with an IEP", value: record.iep_n_size, color: colors.assessment.iep },
    { key: "Students without an IEP", value: record.non_iep_n_size, color: colors.assessment.non_iep },
  ];
  const total = d3.sum(rows, (row) => row.value || 0);
  if (!total) return "";
  const unit = 20;
  const details = `
    <div class="selection-detail-row"><span>Content / grade</span><strong>${escapeHtml(content.label)} / ${escapeHtml(gradeLabel(grade))}</strong></div>
    ${selectionDetailRows(rows, total)}
    <p class="selection-note">MAP-tested student composition for this content/grade, not full-school enrollment.</p>
  `;
  return selectionChartBlock("Assessment 2025: IEP Composition", selectionWaffleHtml(rows, unit), details);
}

function assessmentBarsHtml(rows) {
  return `
    <div class="assessment-bars" role="img" aria-label="Assessment percentage comparison">
      ${rows.map((row) => `
        <div class="assessment-bar-row">
          <span>${escapeHtml(row.key)}</span>
          <div class="assessment-bar-track">
            <div class="assessment-bar-fill" style="width:${Math.max(0, Math.min(100, row.value))}%;background:${escapeHtml(row.color)}"></div>
          </div>
          <strong>${formatMapPct(row.value)}</strong>
        </div>
      `).join("")}
    </div>
  `;
}

function assessmentDirectCertBlock(assessment) {
  const content = bestAssessmentContent(assessment, "direct_certification");
  if (!content) return "";
  const grade = content.direct_certification.default_grade;
  const record = content.direct_certification.grades[grade];
  const rows = [
    {
      key: "Direct-certified",
      value: record.direct_certified.proficient_or_advanced_pct,
      nSize: record.direct_certified.n_size,
      color: colors.assessment.direct_certified,
    },
    {
      key: "Not direct-certified",
      value: record.not_direct_certified.proficient_or_advanced_pct,
      nSize: record.not_direct_certified.n_size,
      color: colors.assessment.not_direct_certified,
    },
  ].filter((row) => Number.isFinite(row.value));
  if (rows.length < 2) return "";
  const details = `
    <div class="selection-detail-row"><span>Content / grade</span><strong>${escapeHtml(content.label)} / ${escapeHtml(gradeLabel(grade))}</strong></div>
    ${rows.map((row) => `
      <div class="selection-detail-row">
        <span><span class="swatch" style="background:${escapeHtml(row.color)}"></span>${escapeHtml(row.key)} n</span>
        <strong>${formatNumber(row.nSize)}</strong>
      </div>
    `).join("")}
  `;
  return selectionChartBlock("Assessment 2025: Direct Certification", assessmentBarsHtml(rows), details);
}

function assessmentSectionHtml(school) {
  const assessment = school.assessment_2025;
  if (!assessment) {
    return selectionChartBlock("Assessment 2025", `<div class="selection-empty-chart">n/a</div>`, `<p class="selection-note">No MAP-tested school-level data available.</p>`);
  }
  const blocks = [
    assessmentPerformanceBlock(assessment),
    assessmentIepBlock(assessment),
    assessmentDirectCertBlock(assessment),
  ].filter(Boolean);
  if (!blocks.length) return "";
  return blocks.join("");
}

function schoolTooltip(d) {
  const mode = schoolModes.find((item) => item.id === state.schoolMode);
  const gradeSpanHtml = gradeSpanTooltipHtml(d);
  if (d.sector === "private") {
    const address = [d.address, d.city, d.state, d.zip].filter(Boolean).join(", ");
    if (state.schoolMode === "enrollmentGrowth") {
      const data = enrollmentGrowthData(d);
      if (!data) {
        return `
          <strong>${escapeHtml(d.school_name)}</strong>
          <div>${escapeHtml(sectorLabel(d))}${d.school_year ? `, ${escapeHtml(d.school_year)}` : ""}</div>
          ${gradeSpanHtml}
          ${address ? `<div>${escapeHtml(address)}</div>` : ""}
          <div>${mode ? escapeHtml(mode.label) : "Enrollment Growth"}: no enrollment history available.</div>
        `;
      }
      const first = data.history[0];
      const latest = data.history[data.history.length - 1];
      return `
        <strong>${escapeHtml(d.school_name)}</strong>
        <div>${escapeHtml(sectorLabel(d))}${d.school_year ? `, ${escapeHtml(d.school_year)}` : ""}</div>
        ${gradeSpanHtml}
        ${address ? `<div>${escapeHtml(address)}</div>` : ""}
        <div>${mode ? escapeHtml(mode.label) : "Enrollment Growth"}</div>
        <dl>
          <dt>Range</dt><dd>${escapeHtml(enrollmentGrowthRangeLabel(data))}</dd>
          <dt>Enrollment</dt><dd>${formatNumber(first.enrollment)} to ${formatNumber(latest.enrollment)}</dd>
          <dt>Displayed max</dt><dd>${formatNumber(data.maxEnrollment)}</dd>
          <dt>Trend</dt><dd>${escapeHtml(enrollmentGrowthDirectionLabel(data.direction, data.trendAvailable))}</dd>
          <dt>Slope</dt><dd>${Number.isFinite(data.slope) ? `${d3.format("+,.1f")(data.slope)} / year` : "n/a"}</dd>
        </dl>
      `;
    }
    const rows = schoolCategories(d);
    const denominator = d3.sum(rows, (row) => row.value || 0);
    const unavailable = privateUnavailableMessage();
    return `
      <strong>${escapeHtml(d.school_name)}</strong>
      <div>${escapeHtml(sectorLabel(d))}${d.school_year ? `, ${escapeHtml(d.school_year)}` : ""}</div>
      ${gradeSpanHtml}
      ${address ? `<div>${escapeHtml(address)}</div>` : ""}
      <div>${mode ? escapeHtml(mode.label) : "School metric"}</div>
      <dl>
        <dt>Enrollment</dt><dd>${formatNumber(d.enrollment)}</dd>
        ${rows.length
          ? rows.map((row) => `<dt>${escapeHtml(row.key)}</dt><dd>${formatTooltipValue(row.value, denominator > 0 ? (row.value || 0) / denominator : NaN)}</dd>`).join("")
          : `<dt>Metric</dt><dd>${escapeHtml(unavailable)}</dd>`}
        <dt>Area</dt><dd>${escapeHtml(d.area_context?.label || "n/a")}</dd>
      </dl>
    `;
  }
  if (state.schoolMode === "enrollmentGrowth") {
    const data = enrollmentGrowthData(d);
    const closureStatus = selectedClosureStatus(d);
    const address = d.sector === "charter" ? [d.address, d.city, d.state, d.zip].filter(Boolean).join(", ") : "";
    if (!data) {
      return `
        <strong>${escapeHtml(d.school_name)}</strong>
        <div>${escapeHtml(sectorLabel(d))}</div>
        ${gradeSpanHtml}
        ${address ? `<div>${escapeHtml(address)}</div>` : ""}
        ${closureStatus ? `<div>${escapeHtml(closureStatus)}</div>` : ""}
        <div>${mode ? escapeHtml(mode.label) : "Enrollment Growth"}: no enrollment history available.</div>
      `;
    }
    const first = data.history[0];
    const latest = data.history[data.history.length - 1];
    return `
      <strong>${escapeHtml(d.school_name)}</strong>
      <div>${escapeHtml(d.sector === "charter" ? sectorLabel(d) : (d.current_function || d.school_type || d.program_category))}</div>
      ${gradeSpanHtml}
      ${address ? `<div>${escapeHtml(address)}</div>` : ""}
      ${closureStatus ? `<div>${escapeHtml(closureStatus)}</div>` : ""}
      <div>${mode ? escapeHtml(mode.label) : "Enrollment Growth"}</div>
      <dl>
        <dt>Range</dt><dd>${escapeHtml(enrollmentGrowthRangeLabel(data))}</dd>
        <dt>Enrollment</dt><dd>${formatNumber(first.enrollment)} to ${formatNumber(latest.enrollment)}</dd>
        <dt>Displayed max</dt><dd>${formatNumber(data.maxEnrollment)}</dd>
        <dt>Trend</dt><dd>${escapeHtml(enrollmentGrowthDirectionLabel(data.direction, data.trendAvailable))}</dd>
        <dt>Slope</dt><dd>${Number.isFinite(data.slope) ? `${d3.format("+,.1f")(data.slope)} / year` : "n/a"}</dd>
      </dl>
    `;
  }
  if (d.sector === "charter") {
    const address = [d.address, d.city, d.state, d.zip].filter(Boolean).join(", ");
    const rows = schoolCategories(d);
    const denominator = d3.sum(rows, (row) => row.value || 0);
    return `
      <strong>${escapeHtml(d.school_name)}</strong>
      <div>${escapeHtml(sectorLabel(d))}${d.source_year ? `, ${escapeHtml(d.source_year)}` : ""}</div>
      ${gradeSpanHtml}
      ${address ? `<div>${escapeHtml(address)}</div>` : ""}
      <div>${mode ? escapeHtml(mode.label) : "School metric"}</div>
      <dl>
        <dt>NCES ID</dt><dd>${escapeHtml(d.nces_school_id || "n/a")}</dd>
        ${rows.map((row) => `<dt${row.description ? ` title="${escapeHtml(row.description)}"` : ""}>${escapeHtml(row.key)}</dt><dd>${formatTooltipValue(row.value, denominator > 0 ? (row.value || 0) / denominator : NaN)}</dd>`).join("")}
      </dl>
    `;
  }
  if (state.schoolMode === "pto") {
    const meta = ptoStatusMeta(d.pto_status);
    const closureStatus = selectedClosureStatus(d);
    return `
      <strong>${escapeHtml(d.school_name)}</strong>
      <div>${escapeHtml(d.current_function || d.school_type || d.program_category)}</div>
      ${gradeSpanHtml}
      ${closureStatus ? `<div>${escapeHtml(closureStatus)}</div>` : ""}
      <div>${mode ? mode.label : "PTO Activity"}</div>
      <dl>
        <dt>Status</dt><dd>${escapeHtml(meta.label)}</dd>
        ${d.pto_status === "unknown" ? "" : `
          <dt>Confidence</dt><dd>${escapeHtml(d.pto_confidence || "unknown")}</dd>
          <dt>Source</dt><dd>${escapeHtml(d.pto_source_label || "n/a")}</dd>
        `}
      </dl>
    `;
  }
  const assessmentMarker = assessmentMarkerData(d);
  if (assessmentMarker) {
    const closureStatus = selectedClosureStatus(d);
    const total = d3.sum(assessmentMarker.rows, (row) => row.value || 0);
    const detailRows = assessmentMarker.rows.map((row) => {
      const value = assessmentMarker.valueKind === "performance_count"
        ? `${formatNumber(row.value)} (${formatMapPct(row.pct)})`
        : formatTooltipValue(row.value, total > 0 ? (row.value || 0) / total : NaN);
      return `<dt>${escapeHtml(row.key)}</dt><dd>${value}</dd>`;
    }).join("");
    return `
      <strong>${escapeHtml(d.school_name)}</strong>
      <div>${escapeHtml(d.current_function || d.school_type || d.program_category)}</div>
      ${gradeSpanHtml}
      ${closureStatus ? `<div>${escapeHtml(closureStatus)}</div>` : ""}
      <div>${mode ? mode.label : "Assessment"}: ${escapeHtml(assessmentMarker.label)}</div>
      <dl>
        <dt>Tested students</dt><dd>${formatNumber(assessmentMarker.nSize)}</dd>
        ${assessmentMarker.proficientOrAdvanced === undefined ? "" : `<dt>Proficient or advanced</dt><dd>${formatMapPct(assessmentMarker.proficientOrAdvanced)}</dd>`}
        ${detailRows}
      </dl>
      ${assessmentMarker.note ? `<div>${escapeHtml(assessmentMarker.note)}</div>` : ""}
    `;
  }
  if (state.schoolMode === "assessmentPerformance" || state.schoolMode === "assessmentIep") {
    const closureStatus = selectedClosureStatus(d);
    return `
      <strong>${escapeHtml(d.school_name)}</strong>
      <div>${escapeHtml(d.current_function || d.school_type || d.program_category)}</div>
      ${gradeSpanHtml}
      ${closureStatus ? `<div>${escapeHtml(closureStatus)}</div>` : ""}
      <div>${mode ? mode.label : "Assessment"}: no usable MAP-tested cell available.</div>
    `;
  }
  const rows = schoolCategories(d);
  const denominator = d3.sum(rows, (row) => row.value || 0);
  const closureStatus = selectedClosureStatus(d);
  return `
    <strong>${escapeHtml(d.school_name)}</strong>
    <div>${escapeHtml(d.current_function || d.school_type || d.program_category)}</div>
    ${gradeSpanHtml}
    ${closureStatus ? `<div>${escapeHtml(closureStatus)}</div>` : ""}
    <div>${mode ? mode.label : "School metric"}</div>
    <dl>${rows.map((row) => `<dt${row.description ? ` title="${escapeHtml(row.description)}"` : ""}>${escapeHtml(row.key)}</dt><dd>${formatTooltipValue(row.value, denominator > 0 ? (row.value || 0) / denominator : NaN)}</dd>`).join("")}</dl>
  `;
}

function privateRaceRows(school) {
  const race = school.race || {};
  const counts = race.counts || {};
  const rows = [
    { key: "American Indian/Alaska Native", value: counts.american_indian_alaska_native || 0, color: "#8c6d31" },
    { key: "Asian", value: counts.asian || 0, color: "#5b8c5a" },
    { key: "Black", value: counts.black || 0, color: colors.race[0] },
    { key: "Hispanic", value: counts.hispanic || 0, color: "#c94833" },
    { key: "White", value: counts.white || 0, color: colors.race[1] },
    { key: "Native Hawaiian/Pacific Islander", value: counts.native_hawaiian_pacific_islander || 0, color: "#2f7f92" },
    { key: "Two or more races", value: counts.two_or_more_races || 0, color: "#7a5aa6" },
  ];
  const reported = d3.sum(rows, (row) => row.value || 0);
  const unreported = Math.max(0, displaySafeCount(race.total) - reported);
  if (unreported > 0) {
    rows.push({ key: "Other / unreported", value: unreported, color: colors.race[2] });
  }
  return rows;
}

function privateRaceGlyphRows(school) {
  const counts = school.race?.counts || {};
  const sourceCounts = [
    counts.black,
    counts.white,
    counts.american_indian_alaska_native,
    counts.asian,
    counts.hispanic,
    counts.native_hawaiian_pacific_islander,
    counts.two_or_more_races,
  ];
  if (!school.availability?.race || !sourceCounts.some(Number.isFinite)) return [];
  const other = Number.isFinite(counts.other)
    ? displaySafeCount(counts.other)
    : displaySafeCount(counts.american_indian_alaska_native)
      + displaySafeCount(counts.asian)
      + displaySafeCount(counts.hispanic)
      + displaySafeCount(counts.native_hawaiian_pacific_islander)
      + displaySafeCount(counts.two_or_more_races);
  return [
    { key: "Black", value: displaySafeCount(counts.black), color: colors.race[0] },
    { key: "White", value: displaySafeCount(counts.white), color: colors.race[1] },
    { key: "Other", value: other, color: colors.race[2] },
  ].filter((row) => row.value > 0);
}

function privateEnrollmentGlyphRows(school) {
  if (!school.availability?.enrollment || !Number.isFinite(school.enrollment) || school.enrollment <= 0) return [];
  return [
    { key: "Campus enrollment", value: school.enrollment, color: colors.publicPrivateEnrollment.private },
  ];
}

function privateUnavailableMessage(mode = state.schoolMode) {
  if (mode === "race") return "No PSS race counts available.";
  if (mode === "enrollmentGrowth") return "No enrollment history is available.";
  if (mode === "assessmentPerformance" || mode === "assessmentIep") return "No private-school MAP display is available.";
  if (mode === "poverty") return "No school-level private direct-certification metric is sourced.";
  if (mode === "pto") return "No private-school PTO metric is sourced.";
  return "No private-school metric is sourced.";
}

function privateSchoolGlyphRows(school, mode = state.schoolMode) {
  if (mode === "race") {
    return { rows: privateRaceGlyphRows(school), unavailable: privateUnavailableMessage(mode) };
  }
  if (mode === "attendance") {
    return { rows: privateEnrollmentGlyphRows(school), unavailable: privateUnavailableMessage(mode) };
  }
  if (mode === "enrollmentGrowth") {
    return { rows: [], unavailable: privateUnavailableMessage(mode) };
  }
  return { rows: [], unavailable: privateUnavailableMessage(mode) };
}

function setPrivateGlyphHalo(g, width, height) {
  const padding = 2.6;
  g.select(".private-school-halo")
    .attr("x", -width / 2 - padding)
    .attr("y", -height / 2 - padding)
    .attr("width", width + padding * 2)
    .attr("height", height + padding * 2)
    .attr("rx", 3.2);
}

function renderPrivateUnavailableMarker(g) {
  const radius = 7.2;
  setPrivateGlyphHalo(g, radius * 2, radius * 2);
  g.append("circle")
    .attr("class", "private-school-unavailable")
    .attr("r", radius);
  g.append("line")
    .attr("class", "private-school-unavailable-slash")
    .attr("x1", -4.2)
    .attr("y1", 4.2)
    .attr("x2", 4.2)
    .attr("y2", -4.2);
}

function renderPrivateSchoolGlyph(g, school) {
  g.selectAll(".waffle-square,.private-school-marker,.private-school-unavailable,.private-school-unavailable-slash,.enrollment-growth-bar,.enrollment-growth-unavailable,.enrollment-growth-unavailable-slash").remove();
  if (state.schoolMode === "enrollmentGrowth") {
    const data = enrollmentGrowthData(school);
    const width = data ? data.history.length * 1.45 + (data.history.length - 1) * 0.36 : 14.8;
    setPrivateGlyphHalo(g, width, 14);
    renderEnrollmentGrowthBars(g, school);
    return;
  }
  const glyph = privateSchoolGlyphRows(school);
  const squares = roundedSquares(glyph.rows, PUBLIC_PRIVATE_ENROLLMENT_WAFFLE_UNIT);
  if (!squares.length) {
    renderPrivateUnavailableMarker(g);
    return;
  }
  const size = 2.9;
  const gap = 0.45;
  const columns = Math.max(3, Math.ceil(Math.sqrt(Math.max(1, squares.length))));
  const rowsCount = Math.ceil(squares.length / columns);
  const width = columns * size + (columns - 1) * gap;
  const height = rowsCount * size + (rowsCount - 1) * gap;
  const x0 = -width / 2;
  const y0 = -height / 2;
  setPrivateGlyphHalo(g, width, height);
  g.selectAll(".waffle-square")
    .data(squares)
    .join("rect")
    .attr("class", "waffle-square private-school-waffle-square")
    .attr("width", size)
    .attr("height", size)
    .attr("rx", 0.4)
    .attr("x", (_, i) => x0 + (i % columns) * (size + gap))
    .attr("y", (_, i) => y0 + Math.floor(i / columns) * (size + gap))
    .attr("fill", (d) => d.color);
}

const descriptions = {
    attendance: `
    <b>90%+ attendance:</b> the share of enrolled students who had 90% attendance rate in 2025
    <br/>
    <b>Enrolled below cutoff:</b> the share of enrolled students had below 90% attendance rate in 2025
    <br/>
    <b>Empty capacity:</b> the amount of capacity in excess of enrollment, based on NCES data
    `,
    deep_poverty: `
    <b>Deep poverty:</b> the share of students who are receiving benefits (such as SNAP), or are homeless or in the foster care system.
    `,
    pto: `
    <b>PTO Activity:</b> based on best-effort web searching. If you have information about PTO acitivity for one of the schools, please <button style="text-decoration:underline; border: none; background: none; cursor: pointer; padding: 0; margin: 0" onclick="window.showFeedbackModal()">reach out</a>.
    `
}

function privateAreaContextBlock(context) {
  if (!context) return "";
  const raceRows = [
    { key: "Black alone", value: context.school_age_race?.black || 0, color: colors.schoolAgeRace.black },
    { key: "White, non-Hispanic", value: context.school_age_race?.white || 0, color: colors.schoolAgeRace.white },
    { key: "Other / residual", value: context.school_age_race?.other || 0, color: colors.schoolAgeRace.other },
  ];
  const enrollmentRows = [
    { key: "Public school", value: context.resident_enrollment_5_17?.public || 0, color: colors.publicPrivateEnrollment.public },
    { key: "Private school", value: context.resident_enrollment_5_17?.private || 0, color: colors.publicPrivateEnrollment.private },
  ];
  const schoolAgeTotal = context.school_age_5_17 || d3.sum(raceRows, (row) => row.value || 0);
  const enrollmentTotal = d3.sum(enrollmentRows, (row) => row.value || 0);
  const label = context.scope === "tract" ? "ACS Tract Context" : "ACS City Context";
  return `
    <section class="selection-chart-block">
      <h3>${label}</h3>
      <div class="selection-plan-list">
        <div class="selection-detail-row"><span>Area</span><strong>${escapeHtml(context.label || "n/a")}</strong></div>
        <div class="selection-detail-row"><span>Poverty rate</span><strong>${formatPct(context.poverty_rate)}</strong></div>
        <div class="selection-detail-row"><span>School-age residents</span><strong>${formatNumber(schoolAgeTotal)}</strong></div>
      </div>
      ${selectionChartBlock("School-Age Race", selectionWaffleHtml(raceRows, 50), selectionDetailRows(raceRows, schoolAgeTotal))}
      ${selectionChartBlock("Resident Public vs. Private Enrollment", selectionWaffleHtml(enrollmentRows, 20), selectionDetailRows(enrollmentRows, enrollmentTotal))}
      <p class="selection-note">ACS context is based on residents near the campus, not students enrolled at the private school.</p>
    </section>
  `;
}

function privateUnavailableRows() {
  const rows = [
    ["Attendance", "n/a"],
    ["Building capacity", "n/a"],
    ["School-level deep poverty", "n/a"],
    ["MAP performance", "n/a"],
    ["IEP distribution", "n/a"],
  ];
  return `
    <section class="selection-chart-block">
      <h3>Unavailable Metrics</h3>
      <div class="selection-plan-list">
        ${rows.map(([label, value]) => `<div class="selection-detail-row"><span>${escapeHtml(label)}</span><strong>${escapeHtml(value)}</strong></div>`).join("")}
      </div>
      <p class="selection-note">These fields are not available from the current NCES EDGE/PSS private-school source contract and are not inferred from ACS or public-school datasets.</p>
    </section>
  `;
}

function charterUnavailableRows(school) {
  const rows = [
    ["Future Ready closure planning", "n/a"],
    ["SLPS assignment-region comparison", "n/a"],
    ["PAR attendance", "n/a"],
    ["Building capacity", "n/a"],
    ["PTO", "n/a"],
  ];
  if (!hasMetric(school, "map_performance") && !hasMetric(school, "iep_distribution")) {
    rows.splice(4, 0, ["MAP/IEP", "n/a"]);
  }
  return `
    <section class="selection-chart-block">
      <h3>Unavailable SLPS Metrics</h3>
      <div class="selection-plan-list">
        ${rows.map(([label, value]) => `<div class="selection-detail-row"><span>${escapeHtml(label)}</span><strong>${escapeHtml(value)}</strong></div>`).join("")}
      </div>
      <p class="selection-note">These fields are not available from the current NCES charter campus source and are not inferred from SLPS-only or ACS resident datasets.</p>
    </section>
  `;
}

function renderSelectedCharterSchool(d) {
  const address = [d.address, d.city, d.state, d.zip].filter(Boolean).join(", ");
  const povertyRows = schoolCategories(d, "poverty");
  const raceRows = schoolCategories(d, "race");
  const povertyTotal = d3.sum(povertyRows, (row) => row.value || 0);
  const raceTotal = d.race_total || d3.sum(raceRows, (row) => row.value || 0);
  const identityRows = [
    ["Address", address || "n/a"],
    ["NCES school ID", d.nces_school_id || "n/a"],
    ["NCES agency", [d.nces_lea_name, d.nces_lea_id].filter(Boolean).join(" / ") || "n/a"],
    ["DESE building", d.dese_map_key ? [d.dese_district_name, d.dese_school_code].filter(Boolean).join(" / ") : "n/a"],
    ["MAP crosswalk", d.map_crosswalk_status ? `${d.map_crosswalk_status}${d.map_crosswalk_confidence ? ` / ${d.map_crosswalk_confidence}` : ""}` : "n/a"],
    ["Source year", d.source_year || "n/a"],
    ["Status", d.school_status_text || d.school_status || "n/a"],
    ["Type", d.school_type_text || "n/a"],
    ["Level", d.school_level || d.school_type || "n/a"],
    ["Grade span", d.grade_span || "n/a"],
    ["Campus enrollment", formatNumber(d.campus_enrollment || d.enrollment_2025)],
    ["Direct-certified", formatNumber(d.direct_cert_count)],
    ["Not direct-certified", formatNumber(d.not_direct_cert_count)],
    ["Source", d.source_url ? linkHtml(d.source_url, "NCES EDGE public school layer") : "n/a"],
  ];
  selection.html(`
    <strong>${escapeHtml(d.school_name)}</strong>
    <div class="selection-subtitle">Charter public school</div>
    <section class="selection-chart-block">
      <h3>Campus</h3>
      <div class="selection-plan-list">
        ${identityRows.map(([label, value]) => `<div class="selection-detail-row"><span>${escapeHtml(label)}</span><strong>${typeof value === "string" && value.startsWith("<a ") ? value : escapeHtml(value)}</strong></div>`).join("")}
      </div>
    </section>
    ${hasMetric(d, "direct_certification")
      ? selectionChartBlock("Direct Certification", selectionWaffleHtml(povertyRows), selectionDetailRows(povertyRows, povertyTotal))
      : selectionChartBlock("Direct Certification", `<div class="selection-empty-chart">n/a</div>`, `<p class="selection-note">NCES did not provide usable direct-certification counts for this campus.</p>`)}
    ${hasMetric(d, "race")
      ? selectionChartBlock("NCES Race", selectionWaffleHtml(raceRows), selectionDetailRows(raceRows, raceTotal))
      : selectionChartBlock("NCES Race", `<div class="selection-empty-chart">n/a</div>`, `<p class="selection-note">NCES did not provide usable race counts for this campus.</p>`)}
    ${selectionChartBlock("Enrollment Growth", selectionEnrollmentGrowthHtml(d), selectionEnrollmentGrowthDetails(d))}
    ${d.assessment_2025 ? assessmentSectionHtml(d) : ""}
    ${charterUnavailableRows(d)}
    <p class="selection-note">Charter campus metrics are CCD school-site records. They are different from ACS residence-based public/private enrollment layers.</p>
  `);
}

function renderSelectedPrivateSchool(d) {
  const address = [d.address, d.city, d.state, d.zip].filter(Boolean).join(", ");
  const identityRows = [
    ["Address", address || "n/a"],
    ["School year", d.school_year || "n/a"],
    ["PPIN", d.ppin || "n/a"],
    ["Grade span", d.grade_span || "n/a"],
    ["Enrollment", formatNumber(d.enrollment)],
    ["Non-PK enrollment", formatNumber(d.enrollment_non_pk)],
    ["Teacher FTE", d.teacher_fte === null || d.teacher_fte === undefined ? "n/a" : d3.format(",.1f")(d.teacher_fte)],
    ["Student/teacher ratio", d.student_teacher_ratio === null || d.student_teacher_ratio === undefined ? "n/a" : d3.format(",.1f")(d.student_teacher_ratio)],
    ["Student body", d.student_body || "n/a"],
    ["School level", d.school_level || "n/a"],
    ["School type", d.school_type || d.program || "n/a"],
    ["Affiliation", d.religious_affiliation || d.orientation || "n/a"],
    ["Associations", d.associations || "n/a"],
    ["Days in year", formatNumber(d.school_days)],
    ["Hours in day", d.student_day_hours === null || d.student_day_hours === undefined ? "n/a" : d3.format(",.1f")(d.student_day_hours)],
    ["Library", d.library || "n/a"],
  ];
  const raceRows = privateRaceRows(d);
  const raceTotal = d.race?.total || d3.sum(raceRows, (row) => row.value || 0);
  selection.html(`
    <strong>${escapeHtml(d.school_name)}</strong>
    <div class="selection-subtitle">Private school</div>
    <section class="selection-chart-block">
      <h3>Campus</h3>
      <div class="selection-plan-list">
        ${identityRows.map(([label, value]) => `<div class="selection-detail-row"><span>${escapeHtml(label)}</span><strong>${escapeHtml(value)}</strong></div>`).join("")}
      </div>
    </section>
    ${d.availability?.race
      ? selectionChartBlock("PSS Race", selectionWaffleHtml(raceRows, 20), selectionDetailRows(raceRows, raceTotal))
      : selectionChartBlock("PSS Race", `<div class="selection-empty-chart">n/a</div>`, `<p class="selection-note">No matched PSS enrichment row is available for this campus.</p>`)}
    ${privateAreaContextBlock(d.area_context)}
    ${privateUnavailableRows()}
  `);
}

function renderSelectedSchool(d) {
  if (d.sector === "charter") {
    renderSelectedCharterSchool(d);
    return;
  }
  const closureStatus = selectedClosureStatus(d);
  const ptoMeta = ptoStatusMeta(d.pto_status);
  const areaComparison = d.area_comparison;
  const attendanceRows = schoolCategories(d, "attendance");
  const povertyRows = schoolCategories(d, "poverty");
  const raceRows = schoolCategories(d, "race");
  const attendanceTotal = d3.sum(attendanceRows, (row) => row.value || 0);
  const povertyTotal = d3.sum(povertyRows, (row) => row.value || 0);
  const raceTotal = d3.sum(raceRows, (row) => row.value || 0);
  const planRows = [
    ["Plan A", d.model_a_status || "n/a"],
    ["Plan B", d.model_b_status || "n/a"],
    ["Plan C", d.model_c_status || "n/a"],
  ];
  selection.html(`
    <strong>${escapeHtml(d.school_name)}</strong>
    <div class="selection-subtitle">${escapeHtml(d.current_function || d.school_type || d.program_category)}</div>
    ${closureStatus ? `<div class="selection-status">${escapeHtml(closureStatus)}</div>` : ""}
    ${selectionChartBlock("Enrollment / Attendance / Capacity", selectionWaffleHtml(attendanceRows), selectionDetailRows(attendanceRows, attendanceTotal))}
    <br/>
    ${descriptions.attendance}
    <br/>
    ${selectionChartBlock("Deep Poverty", selectionWaffleHtml(povertyRows), selectionPovertyDetailRows(povertyRows, povertyTotal, areaComparison))}
    <br/>
    ${descriptions.deep_poverty}
    <br/>
    ${selectionChartBlock("Race", selectionWaffleHtml(raceRows), selectionRaceDetailRows(raceRows, raceTotal, areaComparison))}
    <br/>
    ${areaComparisonNote(areaComparison)}
    ${selectionChartBlock("Enrollment Growth", selectionEnrollmentGrowthHtml(d), selectionEnrollmentGrowthDetails(d))}
    ${assessmentSectionHtml(d)}
    ${selectionChartBlock("PTO Activity", selectionPtoHtml(d), ptoDetailRows(d, ptoMeta))}
    <br/>
    ${descriptions.pto}
    <br/>
    <section class="selection-chart-block">
      <h3>Closure Plans</h3>
      <div class="selection-plan-list">
        ${planRows.map(([label, value]) => `<div class="selection-detail-row"><span>${escapeHtml(label)}</span><strong>${escapeHtml(value)}</strong></div>`).join("")}
      </div>
    </section>
    <p>${d.capacity_match_note || "Capacity note unavailable."}</p>
  `);
}

function selectSchool(d) {
  state.sidebarComparison = "selection";
  updateControls();
  const key = schoolKey(d);
  if (state.selectedSchoolKey === key) {
    state.selectedSchoolKey = null;
    renderSchools();
    renderSummarySelection();
    return;
  }
  state.selectedPrivateSchoolKey = null;
  state.selectedSchoolKey = key;
  renderSchools();
  renderPrivateSchools();
  renderSelectedSchool(d);
}

function selectPrivateSchool(d) {
  state.sidebarComparison = "selection";
  updateControls();
  if (state.selectedPrivateSchoolKey === d.key) {
    state.selectedPrivateSchoolKey = null;
    renderPrivateSchools();
    renderSummarySelection();
    return;
  }
  state.selectedSchoolKey = null;
  state.selectedPrivateSchoolKey = d.key;
  renderSchools();
  renderPrivateSchools();
  renderSelectedPrivateSchool(d);
}

function selectArea(html) {
  state.selectedSchoolKey = null;
  state.selectedPrivateSchoolKey = null;
  renderSchools();
  renderPrivateSchools();
  selection.html(html);
}

function showTooltip(event, html) {
  tooltip.node().hidden = false;
  tooltip.html(html)
    .style("left", `${Math.min(window.innerWidth - 300, event.clientX + 12)}px`)
    .style("top", `${event.clientY + 12}px`);
}

function hideTooltip() {
  tooltip.node().hidden = true;
}

function assessmentLegendRows(mode) {
  if (mode === "assessmentPerformance") {
    return [
      ["Below Basic", colors.assessment.below_basic],
      ["Basic", colors.assessment.basic],
      ["Proficient", colors.assessment.proficient],
      ["Advanced", colors.assessment.advanced],
      ["Missing", colors.assessment.missing],
    ];
  }
  if (mode === "assessmentIep") {
    return [
      ["Students with an IEP", colors.assessment.iep],
      ["Students without an IEP", colors.assessment.non_iep],
    ];
  }
  return [];
}

function updateLegendCollapse() {
  legendPanel.classed("is-collapsed", state.legendCollapsed);
  legend.attr("hidden", state.legendCollapsed ? true : null);
  legendToggle
    .attr("aria-expanded", !state.legendCollapsed)
    .attr("aria-label", state.legendCollapsed ? "Expand legend" : "Minimize legend")
    .attr("title", state.legendCollapsed ? "Expand legend" : "Minimize legend")
    .text(state.legendCollapsed ? "+" : "-");
}

function renderLegend() {
  updateLegendCollapse();
  let rows = [];
  if (state.schoolLevel === "none") {
    legend.html(`
      <div>School symbols hidden.</div>
      ${privateSchoolLegendHtml()}
      ${backgroundLegendHtml()}
    `);
    return;
  }
  if (state.schoolMode === "attendance") {
    rows = schoolCategories({}, "attendance").map((d) => [d.key, d.color]);
  } else if (state.schoolMode === "poverty") {
    rows = schoolCategories({}, "poverty").map((d) => [d.key, d.color]);
  } else if (state.schoolMode === "race") {
    rows = schoolCategories({}, "race").map((d) => [d.key, d.color]);
  } else if (state.schoolMode === "enrollmentGrowth") {
    rows = [
      ["Growing", colors.enrollmentGrowth.growing],
      ["Steady", colors.enrollmentGrowth.steady],
      ["Shrinking", colors.enrollmentGrowth.shrinking],
      ["No history", colors.enrollmentGrowth.unavailable],
    ];
  } else if (state.schoolMode === "assessmentPerformance" || state.schoolMode === "assessmentIep") {
    rows = assessmentLegendRows(state.schoolMode);
  } else {
    rows = Object.entries(ptoStatuses).map(([status, meta]) => [meta.label, colors.pto[status]]);
  }
  const modeNote = state.schoolMode === "pto"
    ? "School symbols show curated public evidence of PTO activity."
    : state.schoolMode === "enrollmentGrowth"
      ? "School symbols show up to 10 annual enrollment bars scaled to each school's own maximum; color shows the least-squares trend."
    : state.schoolMode === "assessmentPerformance"
      ? "School symbols show MAP performance waffles; one square represents 20 approximate tested records."
      : state.schoolMode === "assessmentIep"
        ? "School symbols show MAP-tested IEP composition waffles; one square represents 20 aggregated tested records across available paired cells."
        : "Schools: one square = 20 students.";
  legend.html(`
    <div>${modeNote}</div>
    ${publicSchoolLegendHtml()}
    ${rows.map(([label, color]) => `<div class="legend-row"${state.schoolMode === "poverty" ? ` title="${escapeHtml(deepPovertyTooltip)}"` : ""}><span class="swatch" style="background:${color}"></span><span>${label}</span></div>`).join("")}
    ${state.closurePlan === "none" ? "" : `<div class="legend-row"><span class="swatch" style="background:linear-gradient(135deg, transparent 43%, ${colors.closure} 45%, ${colors.closure} 55%, transparent 57%);opacity:0.7"></span><span>Faded/slashed: closed or repurposed in selected plan</span></div>`}
    ${privateSchoolLegendHtml()}
    ${backgroundLegendHtml()}
  `);
}

function publicSchoolLegendHtml() {
  const shown = schoolSectors.filter((sector) => state.schoolSectors[sector.id]);
  if (!shown.length) {
    return `<div class="legend-block"><div class="legend-title">Public Schools</div><div>Hidden</div></div>`;
  }
  return `
    <div class="legend-block">
      <div class="legend-title">Public Schools</div>
      ${shown.map((sector) => `<div class="legend-row"><span class="swatch" style="background:${colors.sectors[sector.id]}"></span><span>${escapeHtml(sector.label)}</span></div>`).join("")}
    </div>
  `;
}

function privateSchoolLegendHtml() {
  if (!state.showPrivateSchools) {
    return `<div class="legend-block"><div class="legend-title">Private Schools</div><div>Hidden</div></div>`;
  }
  let rows = "";
  let note = "";
  if (state.schoolMode === "attendance") {
    rows = `<div class="legend-row"><span class="swatch private-school-enrollment-swatch"></span><span>PSS campus enrollment</span></div>`;
    note = `One square = ${formatNumber(PUBLIC_PRIVATE_ENROLLMENT_WAFFLE_UNIT)} enrolled students. Private attendance and capacity are not sourced.`;
  } else if (state.schoolMode === "race") {
    rows = [
      ["Black", colors.race[0]],
      ["White", colors.race[1]],
      ["Other", colors.race[2]],
    ].map(([label, color]) => `<div class="legend-row"><span class="swatch" style="background:${color}"></span><span>PSS ${label}</span></div>`).join("");
    note = `Private race glyphs collapse PSS counts to Black / White / Other; campuses without usable PSS race counts show a neutral no-data marker.`;
  } else if (state.schoolMode === "assessmentPerformance" || state.schoolMode === "assessmentIep") {
    rows = `<div class="legend-row"><span class="swatch private-school-unavailable-swatch"></span><span>No private MAP display</span></div>`;
    note = "Missouri Assessment Program data is not available for private-school campus display in the current source contract.";
  } else if (state.schoolMode === "enrollmentGrowth") {
    rows = [
      ["Growing", colors.enrollmentGrowth.growing],
      ["Steady", colors.enrollmentGrowth.steady],
      ["Shrinking", colors.enrollmentGrowth.shrinking],
      ["No history", colors.enrollmentGrowth.unavailable],
    ].map(([label, color]) => `<div class="legend-row"><span class="swatch" style="background:${color}"></span><span>PSS ${label}</span></div>`).join("");
    note = "Private growth uses PPIN-keyed PSS survey history and requires at least two usable enrollment points.";
  } else {
    rows = `<div class="legend-row"><span class="swatch private-school-unavailable-swatch"></span><span>Private metric unavailable</span></div>`;
    note = "Private-school direct certification, PTO, attendance, and capacity are not sourced in the current data contract.";
  }
  return `
    <div class="legend-block">
      <div class="legend-title">Private Schools</div>
      ${rows}
      <div class="background-note">${escapeHtml(note)}</div>
    </div>
  `;
}

function backgroundLegendHtml() {
  if (!assets) return "";
  return `
    ${backgroundColoringLegendHtml()}
    ${backgroundWaffleLegendHtml()}
  `;
}

function backgroundColoringLegendHtml() {
  const background = backgroundColorings.find((d) => d.id === state.backgroundColoring);
  if (!background || background.id === "none") {
    return `<div class="legend-block"><div class="legend-title">Background Color</div><div>None</div></div>`;
  }

  let title = background.label;
  let minLabel = "Low";
  let maxLabel = "High";
  let gradient = "";
  let detail = "";
  const daaConfig = daaBackgroundConfig();
  const acsNote = `${acsDatasetLabel()} ${acsGeographyLabel()} estimates.`;

  if (daaConfig) {
    const values = (assets.daaBlockGroups?.features || []).map((d) => d.properties[daaConfig.property]).filter(Number.isFinite);
    title = daaConfig.title;
    gradient = gradientStops(daaConfig.interpolator, 7, [0, 1]);
    detail = `<div class="background-note">${escapeHtml(daaConfig.detail)}</div>`;
    if (daaConfig.scale === "diverging") {
      const maxAbs = d3.max(values.map(Math.abs)) || 1;
      minLabel = daaConfig.formatter(-maxAbs);
      maxLabel = daaConfig.formatter(maxAbs);
      detail += `<div class="background-note">Midpoint is zero change.</div>`;
    } else {
      const extent = d3.extent(values);
      minLabel = daaConfig.formatter(extent[0] ?? 0);
      maxLabel = daaConfig.formatter(extent[1] ?? 1);
    }
  } else if (state.backgroundColoring === "race") {
    title = `${acsYear()} ACS Race: Black Share`;
    minLabel = "0%";
    maxLabel = "100%";
    gradient = gradientStops(d3.interpolatePuBuGn, 7, [0, 1]);
    detail = `<div class="background-note">${escapeHtml(acsNote)}</div>`;
  } else if (state.backgroundColoring === "poverty") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.poverty_rate) || 1;
    title = `${acsYear()} ACS Poverty Rate`;
    minLabel = "0%";
    maxLabel = formatPct(max);
    gradient = gradientStops(d3.interpolateOrRd, 7, [0, 1]);
    detail = `<div class="background-note">${escapeHtml(acsNote)}</div>`;
  } else if (state.backgroundColoring === "tornado") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.tornado_damage_area_share) || 1;
    title = "May 16, 2025 Tornado Damage Area";
    minLabel = "0%";
    maxLabel = formatPct(max);
    gradient = gradientStops(d3.interpolateReds, 7, [0, 1]);
    detail = `<div class="background-note">City tornado damage analysis overlaid on ${escapeHtml(acsGeographyLabel().toLowerCase())}s.</div>`;
  } else if (state.backgroundColoring === "home") {
    const values = assets.zips.features.map((d) => d.properties.zhvi_typical_home_value).filter(Number.isFinite);
    const extent = d3.extent(values);
    title = `${zhviSourceNote()} Typical Home Value`;
    minLabel = formatMoney(extent[0]);
    maxLabel = `${formatMoney(HOME_VALUE_SCALE_MAX)}+`;
    gradient = gradientStops(d3.interpolateYlGnBu, 7, [0, 1]);
    detail = `<div class="background-note">ZIP-level Zillow Home Value Index typical home value, not an arithmetic average.</div>`;
  } else if (state.backgroundColoring === "schoolAgeTotal") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.school_age_5_17) || 1;
    title = `${acsYear()} ACS School-Age Children`;
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolateYlOrBr, 7, [0, 1]);
    detail = `<div class="background-note">${escapeHtml(acsNote)}</div>`;
  } else if (state.backgroundColoring === "whiteSchoolAgeChildren") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.school_age_white_non_hispanic_5_17) || 1;
    title = `${acsYear()} ACS White School-Age Children Ages 5-17`;
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolateGreens, 7, [0, 1]);
    detail = `<div class="background-note">Non-Hispanic White alone ${escapeHtml(acsDatasetLabel())} estimate.</div>`;
  } else if (state.backgroundColoring === "blackSchoolAgeChildren") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.school_age_black_alone_5_17) || 1;
    title = `${acsYear()} ACS Black School-Age Children Ages 5-17`;
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolatePuBu, 7, [0, 1]);
    detail = `<div class="background-note">Black alone ${escapeHtml(acsDatasetLabel())} estimate; not limited to non-Hispanic residents.</div>`;
  } else if (state.backgroundColoring === "publicSchoolEnrollment") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.public_school_enrolled_5_17) || 1;
    title = `${acsYear()} ACS Public-School Enrollment Ages 5-17`;
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolateGreens, 7, [0, 1]);
    detail = `<div class="background-note">${escapeHtml(acsDatasetLabel())} residence-based estimate; includes public schools outside SLPS.</div>`;
  } else if (state.backgroundColoring === "privateSchoolEnrollment") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.private_school_enrolled_5_17) || 1;
    title = `${acsYear()} ACS Private-School Enrollment Ages 5-17`;
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolateOranges, 7, [0, 1]);
    detail = `<div class="background-note">${escapeHtml(acsDatasetLabel())} residence-based private-school enrollment estimate.</div>`;
  }

  return `
    <div class="legend-block">
      <div class="legend-title">Background Color: ${title}</div>
      <div class="legend-gradient" style="background:${gradient}"></div>
      <div class="legend-scale"><span>${minLabel}</span><span>${maxLabel}</span></div>
      ${detail}
    </div>
  `;
}

function backgroundWaffleLegendHtml() {
  const background = backgroundWaffles.find((d) => d.id === state.backgroundWaffle);
  if (!background || background.id === "none") {
    return `<div class="legend-block"><div class="legend-title">Background Waffles</div><div>None</div></div>`;
  }

  let title = background.label;
  let detail = "";
  if (state.backgroundWaffle === "schoolAgeRace") {
    title = `${acsYear()} ACS School-Age Race Ages 5-17`;
    detail = `
      <div class="legend-row"><span class="swatch" style="background:${colors.schoolAgeRace.black}"></span><span>Black alone</span></div>
      <div class="legend-row"><span class="swatch" style="background:${colors.schoolAgeRace.white}"></span><span>White, non-Hispanic</span></div>
      <div class="legend-row"><span class="swatch" style="background:${colors.schoolAgeRace.other}"></span><span>Other / residual</span></div>
      <div class="background-note">One square = ${formatNumber(SCHOOL_AGE_RACE_WAFFLE_UNIT)} estimated residents ages 5-17 from ${escapeHtml(acsDatasetLabel())}. Black is Black alone; Other is residual.</div>
    `;
  } else if (state.backgroundWaffle === "publicPrivateSchoolEnrollment") {
    title = `${acsYear()} ACS Public vs. Private Enrollment Ages 5-17`;
    detail = `
      <div class="legend-row"><span class="swatch" style="background:${colors.publicPrivateEnrollment.public}"></span><span>Public school</span></div>
      <div class="legend-row"><span class="swatch" style="background:${colors.publicPrivateEnrollment.private}"></span><span>Private school</span></div>
      <div class="background-note">One square = ${formatNumber(PUBLIC_PRIVATE_ENROLLMENT_WAFFLE_UNIT)} estimated enrolled residents ages 5-17 from ${escapeHtml(acsDatasetLabel())}.</div>
    `;
  }

  return `
    <div class="legend-block">
      <div class="legend-title">Background Waffles: ${title}</div>
      ${detail}
    </div>
  `;
}

function gradientStops(interpolator, steps, domain) {
  const stops = d3.range(steps).map((index) => {
    const t = index / (steps - 1);
    const value = domain[0] + (domain[1] - domain[0]) * t;
    return `${interpolator(value)} ${Math.round(t * 100)}%`;
  });
  return `linear-gradient(90deg, ${stops.join(", ")})`;
}

function renderMetadata() {
  const summary = assets.schools.summary;
  const privateSummary = assets.privateSchools.summary || {};
  const shownSchools = visibleSchools().length;
  const shownPrivateSchools = state.showPrivateSchools ? visiblePrivateSchools().length : 0;
  const shownSlps = visibleSchools().filter((school) => school.sector !== "charter").length;
  const shownCharters = visibleSchools().filter((school) => school.sector === "charter").length;
  const levelLabel = state.schoolLevel === "all" ? "all levels" : state.schoolLevel === "none" ? "schools hidden" : state.schoolLevel;
  const closureCount = state.closurePlan === "none"
    ? null
    : visibleSchools().filter(isClosedUnderSelectedPlan).length;
  const notes = assets.metadata.caveats || [];
  d3.select("#metadata").html(`
    <div>${formatNumber(shownSchools)} of ${formatNumber(summary.plotted_schools)} plotted public schools shown (${levelLabel}); ${formatNumber(shownSlps)} SLPS and ${formatNumber(shownCharters)} charter.</div>
    <div>${formatNumber(summary.slps_plotted_schools)} SLPS and ${formatNumber(summary.charter_plotted_schools)} open charter campuses in generated public-school data; ${formatNumber(summary.omitted_missing_coordinates)} SLPS rows omitted without coordinates.</div>
    <div>${formatNumber(shownPrivateSchools)} of ${formatNumber(privateSummary.plotted_schools)} private schools shown; ${formatNumber(privateSummary.pss_enrichment_matched)} have matched PSS enrichment.</div>
    ${closureCount === null ? "" : `<div>${formatNumber(closureCount)} shown schools marked closed/repurposed under the selected plan.</div>`}
    <div>${formatNumber(summary.missing_capacity_count)} plotted schools have missing capacity.</div>
    ${notes.map((note) => `<p>${note}</p>`).join("")}
    <div class="metadata-sources-title">Sources</div>
    <ul class="metadata-source-list">${sourceRowsHtml()}</ul>
  `);
}

function renderAll() {
  fitMap();
  buildScene();
  renderBackground();
  renderNeighborhoods();
  renderRegions();
  renderLabels();
  renderSchools();
  renderPrivateSchools();
  renderLegend();
  renderMetadata();
  refreshSelection();
}

async function loadAssets() {
  const [
    neighborhoods,
    schools,
    privateSchools,
    metadata,
    elementary,
    middle,
    high,
    demographics,
    zips,
    daaBlockGroups,
    tornadoPolygons,
    tornadoPoints,
    tornadoPath,
  ] = await Promise.all([
    d3.json(DATA.neighborhoods),
    d3.json(DATA.schools),
    d3.json(DATA.privateSchools),
    d3.json(DATA.metadata),
    d3.json(DATA.regions.elementary),
    d3.json(DATA.regions.middle),
    d3.json(DATA.regions.high),
    d3.json(DATA.demographics),
    d3.json(DATA.zips),
    d3.json(DATA.daaBlockGroups),
    d3.json(DATA.tornadoPolygons),
    d3.json(DATA.tornadoPoints),
    d3.json(DATA.tornadoPath),
  ]);
  return {
    neighborhoods,
    schools,
    privateSchools,
    metadata,
    regions: { elementary, middle, high },
    demographics,
    zips,
    daaBlockGroups,
    tornadoPolygons,
    tornadoPoints,
    tornadoPath,
  };
}

initFeedbackModal();
initControls();
loadAssets()
  .then((loaded) => {
    assets = loaded;
    loading.style("display", "none");
    renderAll();
    window.addEventListener("resize", () => renderAll());
  })
  .catch((error) => {
    console.error(error);
    loading.text(`Could not load map assets: ${error.message}`);
  });
