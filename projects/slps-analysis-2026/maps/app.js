const DATA = {
  neighborhoods: "data/neighborhoods.geojson",
  schools: "data/schools.json",
  metadata: "data/metadata.json",
  regions: {
    elementary: "data/school_regions_elementary.geojson",
    middle: "data/school_regions_middle.geojson",
    high: "data/school_regions_high.geojson",
  },
  demographics: "data/demographics_tracts.geojson",
  zips: "data/demographics_zips.geojson",
  tornadoPolygons: "data/tornado_damage_polygons.geojson",
  tornadoPoints: "data/tornado_damage_points.geojson",
  tornadoPath: "data/tornado_path.geojson",
};

const HOME_VALUE_SCALE_MAX = 400000;

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
  { id: "elementary", label: "Elementary" },
  { id: "middle", label: "Middle" },
  { id: "high", label: "High" },
];

const closurePlans = [
  { id: "none", label: "None" },
  { id: "model_a_status", label: "Plan A" },
  { id: "model_b_status", label: "Plan B" },
  { id: "model_c_status", label: "Plan C" },
];

const backgrounds = [
  { id: "none", label: "None" },
  { id: "race", label: "Race" },
  { id: "poverty", label: "Poverty Indicators" },
  { id: "tornado", label: "2025 Tornado Damage" },
  { id: "home", label: "Typical Home Value" },
  { id: "schoolAgeTotal", label: "School-Age Children Total" },
  { id: "publicSchoolEnrollment", label: "Public-School Enrollment Ages 5-17" },
  { id: "privateSchoolEnrollment", label: "Private-School Enrollment Ages 5-17" },
];

const backgroundDescriptions = {
  none: "No background layer is drawn; only neighborhoods, selected assignment regions, and school symbols are visible.",
  race: "Census tract ACS race estimates. The color scale shows the share of residents identified as non-Hispanic Black alone.",
  poverty: "Census tract ACS poverty estimates. The color scale shows the share of residents below the federal poverty threshold.",
  tornado: "City tornado damage analysis overlaid on Census tracts, including surveyed damage path and damage observations from the May 16, 2025 tornado.",
  home: "ZIP-level Zillow Home Value Index typical home value. ZHVI is Zillow's typical-value estimate, not an average sale price.",
  schoolAgeTotal: "Census tract ACS count of residents ages 5-17.",
  publicSchoolEnrollment: "Census tract ACS residence-based count of ages 5-17 enrolled in public school, including public schools outside SLPS.",
  privateSchoolEnrollment: "Census tract ACS residence-based count of ages 5-17 enrolled in private school.",
};

const deepPovertyTooltip = "Deep poverty uses the district direct-certification count: students whose household participation in assistance programs directly certifies them for free meals. Not deep poverty is enrollment minus that count.";

const colors = {
  attendance: ["#173f35", "#6f8f7a", "#d9e4cf"],
  poverty: ["#762a83", "#d8c4dc"],
  race: ["#2f3542", "#f1efe7", "#3f88c5"],
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
  background: "publicSchoolEnrollment",
  showRegions: false,
  showNeighborhoods: false,
  selectedSchoolKey: null,
  transform: d3.zoomIdentity,
};

const svg = d3.select("#map");
const loading = d3.select("#loading");
const tooltip = d3.select("#tooltip");
const selection = d3.select("#selection");
const legend = d3.select("#legend");
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
  if (value === null || value === undefined || Number.isNaN(value)) return "n/a";
  return d3.format(",.0f")(value);
}

function formatPct(value) {
  if (value === null || value === undefined || Number.isNaN(value)) return "n/a";
  return d3.format(".0%")(value);
}

function formatMapPct(value) {
  if (value === null || value === undefined || Number.isNaN(value)) return "n/a";
  return `${d3.format(".1f")(value)}%`;
}

function formatTooltipValue(value, share) {
  const shareText = Number.isFinite(share) ? ` (${formatPct(share)})` : "";
  return `${formatNumber(value)}${shareText}`;
}

function formatMoney(value) {
  if (value === null || value === undefined || Number.isNaN(value)) return "n/a";
  return d3.format("$,.0f")(value);
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
  if (mode === "assessmentPerformance") return assessmentMarkerPerformance(school);
  if (mode === "assessmentIep") return assessmentMarkerIep(school);
  return null;
}

function schoolCategories(school, mode = state.schoolMode) {
  if (mode === "attendance") {
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
  if (state.closurePlan === "none") return null;
  return school[state.closurePlan] || null;
}

function isClosedUnderSelectedPlan(school) {
  const status = selectedClosureStatus(school);
  return Boolean(status && /closed|repurposed/i.test(status));
}

function schoolLevelMatches(school) {
  if (state.schoolLevel === "all") return true;
  return (school.school_type || "").toLowerCase() === state.schoolLevel;
}

function visibleSchools() {
  return assets.schools.schools.filter(schoolLevelMatches);
}

function schoolKey(school) {
  return school.school_code || school.school_name;
}

function selectedSchool() {
  if (!state.selectedSchoolKey || !assets) return null;
  return assets.schools.schools.find((school) => schoolKey(school) === state.selectedSchoolKey) || null;
}

function refreshSelection() {
  const school = selectedSchool();
  if (school && schoolLevelMatches(school)) {
    renderSelectedSchool(school);
    return;
  }
  if (school) state.selectedSchoolKey = null;
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
      if (state.schoolLevel === "all") state.showRegions = false;
      updateControls();
      renderSchools();
      renderRegions();
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

  const bg = d3.select("#backgroundControls");
  bg.selectAll("button")
    .data(backgrounds)
    .join("button")
    .attr("type", "button")
    .attr("role", "radio")
    .attr("aria-checked", (d) => d.id === state.background)
    .text((d) => d.label)
    .on("click", (_, d) => {
      state.background = d.id;
      updateControls();
      renderBackground();
      renderLegend();
    });
  bg.append("div")
    .attr("id", "backgroundDescription")
    .attr("class", "background-note");

  d3.select("#schoolGlyphSize").on("input", (event) => {
    state.schoolGlyphScale = Number(event.target.value);
    updateControls();
    positionSchools();
  });
  d3.select("#showRegions").on("change", (event) => {
    state.showRegions = event.target.checked;
    renderRegions();
  });
  d3.select("#showNeighborhoods").on("change", (event) => {
    state.showNeighborhoods = event.target.checked;
    renderLabels();
  });
  updateControls();
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
  d3.select("#schoolMetricControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.schoolMode);
  d3.select("#schoolLevelControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.schoolLevel);
  d3.select("#closurePlanControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.closurePlan);
  d3.select("#backgroundControls")
    .selectAll("button")
    .attr("aria-checked", (d) => d.id === state.background);
  d3.select("#showRegions")
    .property("checked", state.showRegions)
    .property("disabled", state.schoolLevel === "all");
  d3.select("#schoolGlyphSizeValue").text(`${d3.format(".2~f")(state.schoolGlyphScale)}x`);
  d3.select("#backgroundDescription").text(backgroundDescriptions[state.background] || "");
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
  if (state.background === "none") return;

  if (state.background === "home") {
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
      .on("mousemove", (event, d) => showTooltip(event, `<strong>ZIP ${d.properties.city_zip}</strong><br>ZHVI: ${formatMoney(d.properties.zhvi_typical_home_value)}`))
      .on("mouseleave", hideTooltip);
    return;
  }

  const features = assets.demographics.features;
  let value;
  let color;
  if (state.background === "race") {
    value = (d) => d.properties.race_black_share;
    color = d3.scaleSequential(d3.interpolatePuBuGn).domain([0, 1]);
  } else if (state.background === "poverty") {
    value = (d) => d.properties.poverty_rate;
    color = d3.scaleSequential(d3.interpolateOrRd).domain([0, d3.max(features, value) || 1]);
  } else if (state.background === "schoolAgeTotal") {
    value = (d) => d.properties.school_age_5_17;
    color = d3.scaleSequential(d3.interpolateYlOrBr).domain([0, d3.max(features, value) || 1]);
  } else if (state.background === "publicSchoolEnrollment") {
    value = (d) => d.properties.public_school_enrolled_5_17;
    color = d3.scaleSequential(d3.interpolateGreens).domain([0, d3.max(features, value) || 1]);
  } else if (state.background === "privateSchoolEnrollment") {
    value = (d) => d.properties.private_school_enrolled_5_17;
    color = d3.scaleSequential(d3.interpolatePurples).domain([0, d3.max(features, value) || 1]);
  } else if (state.background === "tornado") {
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
    .on("mousemove", (event, d) => showTooltip(event, backgroundTooltip(d)))
    .on("mouseleave", hideTooltip);
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

function backgroundTooltip(d) {
  const p = d.properties;
  if (state.background === "race") return `<strong>${p.name}</strong><br>Black share: ${formatPct(p.race_black_share)}<br>White share: ${formatPct(p.race_white_share)}`;
  if (state.background === "poverty") return `<strong>${p.name}</strong><br>Poverty rate: ${formatPct(p.poverty_rate)}`;
  if (state.background === "schoolAgeTotal") return `<strong>${p.name}</strong><br>School-age children: ${formatNumber(p.school_age_5_17)}`;
  if (state.background === "publicSchoolEnrollment") {
    return `<strong>${p.name}</strong><br>Residents ages 5-17 in public school: ${formatNumber(p.public_school_enrolled_5_17)}<br>Share of school-age residents: ${formatPct(p.public_school_enrolled_5_17_per_school_age)}`;
  }
  if (state.background === "privateSchoolEnrollment") {
    return `<strong>${p.name}</strong><br>Residents ages 5-17 in private school: ${formatNumber(p.private_school_enrolled_5_17)}<br>Share of school-age residents: ${formatPct(p.private_school_enrolled_5_17_per_school_age)}`;
  }
  return `<strong>${p.name}</strong><br>Tornado damage area: ${formatPct(p.tornado_damage_area_share)}<br>Damage points: ${formatNumber(p.tornado_damage_point_count)}`;
}

function renderRegions() {
  const layer = root.select(".region-layer");
  layer.selectAll("*").remove();
  if (!state.showRegions || state.schoolLevel === "all") return;
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
    .data(visibleSchools(), (d) => d.school_code || d.school_name)
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
    .attr("opacity", (d) => isClosedUnderSelectedPlan(d) ? 0.5 : 1);
  groups.select(".school-outline")
    .attr("r", (d) => state.schoolMode === "pto" ? 13 : 12)
    .attr("stroke", (d) => colors.groups[d.display_group] || colors.groups.specialized);

  groups.each(function(d) {
    const g = d3.select(this);
    g.selectAll(".waffle-square,.pto-marker,.pto-status-text,.closure-slash").remove();
    if (state.schoolMode === "pto") {
      renderPtoStatus(g, d);
    } else {
      renderWaffle(g, d);
    }
    renderClosureSlash(g, d);
  });
}

function sumSchoolRows(schools, rows) {
  return rows.map((row) => ({
    ...row,
    value: d3.sum(schools, (school) => school[row.field] || 0),
  }));
}

function summaryAttendanceRows(schools) {
  return sumSchoolRows(schools, [
    { key: "90%+ attendance", field: "attendance_90plus_students", color: colors.attendance[0] },
    { key: "Enrolled below cutoff", field: "attendance_below_90_students", color: colors.attendance[1] },
    { key: "Empty capacity", field: "empty_capacity_seats", color: colors.attendance[2] },
  ]);
}

function summaryPovertyRows(schools) {
  return sumSchoolRows(schools, [
    { key: "Direct-certified", field: "direct_cert_count", color: colors.poverty[0], description: deepPovertyTooltip },
    { key: "Not direct-certified", field: "not_direct_cert_count", color: colors.poverty[1], description: deepPovertyTooltip },
  ]);
}

function summaryRaceRows(schools) {
  return sumSchoolRows(schools, [
    { key: "Black", field: "black_count", color: colors.race[0] },
    { key: "White", field: "white_count", color: colors.race[1] },
    { key: "Other", field: "other_count", color: colors.race[2] },
  ]);
}

function summaryPtoRows(schools) {
  const counts = d3.rollup(schools, (items) => items.length, (school) => school.pto_status || "unknown");
  return Object.entries(ptoStatuses).map(([status, meta]) => ({
    key: meta.label,
    value: counts.get(status) || 0,
    color: colors.pto[status] || colors.pto.unknown,
  }));
}

function summaryMetricBlock(title, rows, unit = 20) {
  const total = d3.sum(rows, (row) => row.value || 0);
  return selectionChartBlock(title, selectionWaffleHtml(rows, unit), selectionDetailRows(rows, total));
}

function summaryKpiRows(schools) {
  const capacitySchools = schools.filter((school) => Number.isFinite(school.official_building_capacity));
  return `
    <div class="summary-kpis">
      <div><span>Schools</span><strong>${formatNumber(schools.length)}</strong></div>
      <div><span>Enrollment</span><strong>${formatNumber(d3.sum(schools, (school) => school.enrollment_2025 || 0))}</strong></div>
      <div><span>Students w/ 90%+ attendance</span><strong>${formatNumber(d3.sum(schools, (school) => school.par_students_total || 0))}</strong></div>
      <div><span>Capacity</span><strong>${formatNumber(d3.sum(schools, (school) => school.official_building_capacity || 0))}</strong></div>
      <div><span>Empty seats</span><strong>${formatNumber(d3.sum(schools, (school) => school.empty_capacity_seats || 0))}</strong></div>
      <div><span>Capacity records</span><strong>${formatNumber(capacitySchools.length)} of ${formatNumber(schools.length)}</strong></div>
    </div>
  `;
}

function summaryGroupHtml(label, schools) {
  return `
    <section class="summary-group">
      <h3>${escapeHtml(label)}</h3>
      ${summaryKpiRows(schools)}
      ${summaryMetricBlock("Enrollment / Attendance / Capacity", summaryAttendanceRows(schools))}
      <br/>
      ${descriptions.attendance}
      <br/>
      ${summaryMetricBlock("Deep Poverty", summaryPovertyRows(schools))}
      <br/>
      ${descriptions.deep_poverty}
      <br/>
      ${summaryMetricBlock("Race", summaryRaceRows(schools))}
      ${summaryMetricBlock("PTO Activity", summaryPtoRows(schools), 1)}
      <br/>
      ${descriptions.pto}
      <br/>
    </section>
  `;
}

function renderSummarySelection() {
  if (!assets) return;
  const schools = visibleSchools();
  const levelLabel = state.schoolLevel === "all" ? "all school levels" : `${state.schoolLevel} schools`;
  const selectedPlan = closurePlans.find((plan) => plan.id === state.closurePlan);
  const groups = state.closurePlan === "none"
    ? [{ label: "Shown schools", schools }]
    : [
        { label: "Remain open", schools: schools.filter((school) => !isClosedUnderSelectedPlan(school)) },
        { label: "Closed or repurposed", schools: schools.filter(isClosedUnderSelectedPlan) },
      ];

  selection.html(`
    <strong>Summary</strong>
    <div class="selection-subtitle">${formatNumber(schools.length)} rendered schools, ${escapeHtml(levelLabel)}${selectedPlan && state.closurePlan !== "none" ? `, ${escapeHtml(selectedPlan.label)}` : ""}</div>
    ${groups.map((group) => summaryGroupHtml(group.label, group.schools)).join("")}
  `);
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
    .attr("transform", (d) => `translate(${projection([d.lon, d.lat])}) scale(${scale})`);
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

function renderPtoStatus(g, school) {
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
    .text(meta.label[0]);
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
      <span>${escapeHtml(meta.label[0])}</span>
    </div>
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
  if (state.schoolMode === "pto") {
    const meta = ptoStatusMeta(d.pto_status);
    const closureStatus = selectedClosureStatus(d);
    return `
      <strong>${escapeHtml(d.school_name)}</strong>
      <div>${escapeHtml(d.current_function || d.school_type || d.program_category)}</div>
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
      ${closureStatus ? `<div>${escapeHtml(closureStatus)}</div>` : ""}
      <div>${mode ? mode.label : "Assessment"}: no usable MAP-tested cell available.</div>
    `;
  }
  const rows = schoolCategories(d);
  const denominator = d3.sum(rows, (row) => row.value || 0);
  const closureStatus = selectedClosureStatus(d);
  return `
    <strong>${d.school_name}</strong>
    <div>${d.current_function || d.school_type || d.program_category}</div>
    ${closureStatus ? `<div>${closureStatus}</div>` : ""}
    <div>${mode ? mode.label : "School metric"}</div>
    <dl>${rows.map((row) => `<dt${row.description ? ` title="${escapeHtml(row.description)}"` : ""}>${row.key}</dt><dd>${formatTooltipValue(row.value, denominator > 0 ? (row.value || 0) / denominator : NaN)}</dd>`).join("")}</dl>
  `;
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

function renderSelectedSchool(d) {
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
  const key = schoolKey(d);
  if (state.selectedSchoolKey === key) {
    state.selectedSchoolKey = null;
    renderSummarySelection();
    return;
  }
  state.selectedSchoolKey = key;
  renderSelectedSchool(d);
}

function selectArea(html) {
  state.selectedSchoolKey = null;
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

function renderLegend() {
  let rows = [];
  if (state.schoolMode === "attendance") {
    rows = schoolCategories({}, "attendance").map((d) => [d.key, d.color]);
  } else if (state.schoolMode === "poverty") {
    rows = schoolCategories({}, "poverty").map((d) => [d.key, d.color]);
  } else if (state.schoolMode === "race") {
    rows = schoolCategories({}, "race").map((d) => [d.key, d.color]);
  } else if (state.schoolMode === "assessmentPerformance" || state.schoolMode === "assessmentIep") {
    rows = assessmentLegendRows(state.schoolMode);
  } else {
    rows = Object.entries(ptoStatuses).map(([status, meta]) => [meta.label, colors.pto[status]]);
  }
  const modeNote = state.schoolMode === "pto"
    ? "School symbols show curated public evidence of PTO activity."
    : state.schoolMode === "assessmentPerformance"
      ? "School symbols show MAP performance waffles; one square represents 20 approximate tested records."
      : state.schoolMode === "assessmentIep"
        ? "School symbols show MAP-tested IEP composition waffles; one square represents 20 aggregated tested records across available paired cells."
        : "School symbols: one square represents 20 students in waffle modes.";
  legend.html(`
    <div>${modeNote}</div>
    ${rows.map(([label, color]) => `<div class="legend-row"${state.schoolMode === "poverty" ? ` title="${escapeHtml(deepPovertyTooltip)}"` : ""}><span class="swatch" style="background:${color}"></span><span>${label}</span></div>`).join("")}
    ${state.closurePlan === "none" ? "" : `<div class="legend-row"><span class="swatch" style="background:linear-gradient(135deg, transparent 43%, ${colors.closure} 45%, ${colors.closure} 55%, transparent 57%);opacity:0.7"></span><span>Faded/slashed: closed or repurposed in selected plan</span></div>`}
    ${backgroundLegendHtml()}
  `);
}

function backgroundLegendHtml() {
  if (!assets) return "";
  const background = backgrounds.find((d) => d.id === state.background);
  if (!background || background.id === "none") {
    return `<div class="legend-block"><div class="legend-title">Background</div><div>None</div></div>`;
  }

  let title = background.label;
  let minLabel = "Low";
  let maxLabel = "High";
  let gradient = "";

  if (state.background === "race") {
    title = "Race: Black Share";
    minLabel = "0%";
    maxLabel = "100%";
    gradient = gradientStops(d3.interpolatePuBuGn, 7, [0, 1]);
  } else if (state.background === "poverty") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.poverty_rate) || 1;
    title = "Poverty Rate";
    minLabel = "0%";
    maxLabel = formatPct(max);
    gradient = gradientStops(d3.interpolateOrRd, 7, [0, 1]);
  } else if (state.background === "tornado") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.tornado_damage_area_share) || 1;
    title = "Tornado Damage Area";
    minLabel = "0%";
    maxLabel = formatPct(max);
    gradient = gradientStops(d3.interpolateReds, 7, [0, 1]);
  } else if (state.background === "home") {
    const values = assets.zips.features.map((d) => d.properties.zhvi_typical_home_value).filter(Number.isFinite);
    const extent = d3.extent(values);
    title = "ZHVI Typical Home Value";
    minLabel = formatMoney(extent[0]);
    maxLabel = `${formatMoney(HOME_VALUE_SCALE_MAX)}+`;
    gradient = gradientStops(d3.interpolateYlGnBu, 7, [0, 1]);
  } else if (state.background === "schoolAgeTotal") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.school_age_5_17) || 1;
    title = "School-Age Children";
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolateYlOrBr, 7, [0, 1]);
  } else if (state.background === "publicSchoolEnrollment") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.public_school_enrolled_5_17) || 1;
    title = "Public-School Enrollment Ages 5-17";
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolateGreens, 7, [0, 1]);
  } else if (state.background === "privateSchoolEnrollment") {
    const max = d3.max(assets.demographics.features, (d) => d.properties.private_school_enrolled_5_17) || 1;
    title = "Private-School Enrollment Ages 5-17";
    minLabel = "0";
    maxLabel = formatNumber(max);
    gradient = gradientStops(d3.interpolatePurples, 7, [0, 1]);
  }

  return `
    <div class="legend-block">
      <div class="legend-title">Background: ${title}</div>
      <div class="legend-gradient" style="background:${gradient}"></div>
      <div class="legend-scale"><span>${minLabel}</span><span>${maxLabel}</span></div>
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
  const shownSchools = visibleSchools().length;
  const levelLabel = state.schoolLevel === "all" ? "all levels" : state.schoolLevel;
  const closureCount = state.closurePlan === "none"
    ? null
    : visibleSchools().filter(isClosedUnderSelectedPlan).length;
  const notes = assets.metadata.caveats || [];
  d3.select("#metadata").html(`
    <div>${formatNumber(shownSchools)} of ${formatNumber(summary.plotted_schools)} plotted schools shown (${levelLabel}); ${formatNumber(summary.omitted_missing_coordinates)} omitted without coordinates.</div>
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
  renderLegend();
  renderMetadata();
  refreshSelection();
}

async function loadAssets() {
  const [
    neighborhoods,
    schools,
    metadata,
    elementary,
    middle,
    high,
    demographics,
    zips,
    tornadoPolygons,
    tornadoPoints,
    tornadoPath,
  ] = await Promise.all([
    d3.json(DATA.neighborhoods),
    d3.json(DATA.schools),
    d3.json(DATA.metadata),
    d3.json(DATA.regions.elementary),
    d3.json(DATA.regions.middle),
    d3.json(DATA.regions.high),
    d3.json(DATA.demographics),
    d3.json(DATA.zips),
    d3.json(DATA.tornadoPolygons),
    d3.json(DATA.tornadoPoints),
    d3.json(DATA.tornadoPath),
  ]);
  return {
    neighborhoods,
    schools,
    metadata,
    regions: { elementary, middle, high },
    demographics,
    zips,
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
