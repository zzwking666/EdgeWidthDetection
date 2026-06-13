#!/usr/bin/env node
// Phase 1: Structural Analysis Script
// Computes directory groupings, import adjacency, density, pattern matching, etc.

const fs = require('fs');
const path = require('path');

const inputPath = process.argv[2];
const outputPath = process.argv[3];

if (!inputPath || !outputPath) {
  console.error('Usage: node ua-arch-analyze.js <input.json> <output.json>');
  process.exit(1);
}

let data;
try {
  data = JSON.parse(fs.readFileSync(inputPath, 'utf-8'));
} catch (e) {
  console.error('Failed to read input:', e.message);
  process.exit(1);
}

const { fileNodes, importEdges, allEdges } = data;

// ---- Helpers ----

// Extract directory from a filePath
function getDir(filePath) {
  const d = path.dirname(filePath);
  return d === '.' ? '(root)' : d.replace(/\\/g, '/');
}

// Split path into segments
function pathSegments(fp) {
  return fp.replace(/\\/g, '/').split('/').filter(s => s.length > 0);
}

// ---- A. Directory Grouping ----

// First figure out the common prefix
const allPaths = fileNodes.map(n => n.filePath.replace(/\\/g, '/'));
const allSegments = allPaths.map(p => p.split('/').filter(s => s.length > 0));

function commonPrefixLength(arrays) {
  if (arrays.length === 0) return 0;
  let len = 0;
  const first = arrays[0];
  while (len < first.length) {
    const seg = first[len];
    if (arrays.every(a => a.length > len && a[len] === seg)) {
      len++;
    } else {
      break;
    }
  }
  return len;
}

const cpl = commonPrefixLength(allSegments);

// Group by first directory segment after common prefix
const directoryGroups = {};
const flatGroups = {}; // For flat structures, group by extension pattern

for (const node of fileNodes) {
  const segments = pathSegments(node.filePath);
  const remaining = segments.slice(cpl);

  let groupKey;
  if (remaining.length > 0) {
    groupKey = remaining[0];
  } else if (segments.length > 0) {
    groupKey = segments[segments.length - 1];
  } else {
    groupKey = '(root)';
  }

  if (!directoryGroups[groupKey]) directoryGroups[groupKey] = [];
  directoryGroups[groupKey].push(node.id);
}

// Detect if structure is flat (all files in same root dir or just one level)
const groupCount = Object.keys(directoryGroups).length;
const maxGroupSize = Math.max(...Object.values(directoryGroups).map(a => a.length));
const isFlat = groupCount <= 3 && maxGroupSize >= fileNodes.length * 0.7;

// For flat structures, also build extension-based groups
// (Not needed here since we have clear subdirectory structure)

// ---- B. Node Type Grouping ----

const nodeTypeGroups = {};
for (const node of fileNodes) {
  const t = node.type || 'file';
  if (!nodeTypeGroups[t]) nodeTypeGroups[t] = [];
  nodeTypeGroups[t].push(node.id);
}

// ---- C. Import Adjacency Matrix ----

// Build maps: fileId -> set of imported fileIds (fan-out), and reverse (fan-in)
const fanOutMap = {};
const fanInMap = {};
for (const node of fileNodes) {
  fanOutMap[node.id] = new Set();
  fanInMap[node.id] = new Set();
}

for (const edge of importEdges) {
  if (edge.type === 'imports') {
    const src = edge.source;
    const tgt = edge.target;
    if (fanOutMap[src] !== undefined && fanInMap[tgt] !== undefined) {
      fanOutMap[src].add(tgt);
      fanInMap[tgt].add(src);
    }
  }
}

const fileFanIn = {};
const fileFanOut = {};
for (const node of fileNodes) {
  fileFanIn[node.id] = fanInMap[node.id] ? fanInMap[node.id].size : 0;
  fileFanOut[node.id] = fanOutMap[node.id] ? fanOutMap[node.id].size : 0;
}

// For each directory group, compute which other groups it imports from / is imported by
const groupImportTargets = {}; // group -> Set of target groups
const groupImportSources = {}; // group -> Set of source groups

// Map fileId -> groupKey
const fileToGroup = {};
for (const [group, ids] of Object.entries(directoryGroups)) {
  for (const id of ids) {
    fileToGroup[id] = group;
  }
}

for (const [group] of Object.entries(directoryGroups)) {
  groupImportTargets[group] = new Set();
  groupImportSources[group] = new Set();
}

for (const edge of importEdges) {
  if (edge.type !== 'imports') continue;
  const srcGroup = fileToGroup[edge.source];
  const tgtGroup = fileToGroup[edge.target];
  if (srcGroup && tgtGroup && srcGroup !== tgtGroup) {
    groupImportTargets[srcGroup].add(tgtGroup);
    groupImportSources[tgtGroup].add(srcGroup);
  }
}

// ---- D. Cross-Category Dependency Analysis ----

const crossCategoryEdges = [];
const edgeTypeCounts = {};

for (const edge of allEdges) {
  const srcNode = fileNodes.find(n => n.id === edge.source);
  const tgtNode = fileNodes.find(n => n.id === edge.target);
  if (!srcNode || !tgtNode) continue;

  const fromType = srcNode.type || 'file';
  const toType = tgtNode.type || 'file';
  const edgeType = edge.type;

  const key = `${fromType}->${toType}:${edgeType}`;
  if (!edgeTypeCounts[key]) edgeTypeCounts[key] = { fromType, toType, edgeType, count: 0 };
  edgeTypeCounts[key].count++;
}

for (const entry of Object.values(edgeTypeCounts)) {
  crossCategoryEdges.push(entry);
}

// ---- E. Inter-Group Import Frequency ----

const interGroupImportCounts = {};
for (const edge of importEdges) {
  if (edge.type !== 'imports') continue;
  const srcGroup = fileToGroup[edge.source];
  const tgtGroup = fileToGroup[edge.target];
  if (!srcGroup || !tgtGroup) continue;

  const key = `${srcGroup}->${tgtGroup}`;
  if (!interGroupImportCounts[key]) interGroupImportCounts[key] = 0;
  interGroupImportCounts[key]++;
}

const interGroupImports = [];
for (const [key, count] of Object.entries(interGroupImportCounts)) {
  const [from, to] = key.split('->');
  interGroupImports.push({ from, to, count });
}

// ---- F. Intra-Group Import Density ----

const intraGroupDensity = {};
for (const [group, ids] of Object.entries(directoryGroups)) {
  const idSet = new Set(ids);
  let internalEdges = 0;
  let totalEdges = 0;

  for (const edge of importEdges) {
    if (edge.type !== 'imports') continue;
    const srcIn = idSet.has(edge.source);
    const tgtIn = idSet.has(edge.target);

    if (srcIn || tgtIn) totalEdges++;
    if (srcIn && tgtIn) internalEdges++;
  }

  intraGroupDensity[group] = {
    internalEdges,
    totalEdges,
    density: totalEdges > 0 ? internalEdges / totalEdges : 0
  };
}

// ---- G. Directory Pattern Matching ----

const dirPatterns = {
  'routes': 'api', 'api': 'api', 'controllers': 'api', 'endpoints': 'api', 'handlers': 'api',
  'services': 'service', 'core': 'service', 'lib': 'service', 'domain': 'service', 'logic': 'service',
  'models': 'data', 'db': 'data', 'data': 'data', 'persistence': 'data', 'repository': 'data', 'entities': 'data',
  'components': 'ui', 'views': 'ui', 'pages': 'ui', 'ui': 'ui', 'layouts': 'ui', 'screens': 'ui',
  'middleware': 'middleware', 'plugins': 'middleware', 'interceptors': 'middleware', 'guards': 'middleware',
  'utils': 'utility', 'helpers': 'utility', 'common': 'utility', 'shared': 'utility', 'tools': 'utility',
  'config': 'config', 'constants': 'config', 'env': 'config', 'settings': 'config',
  '__tests__': 'test', 'test': 'test', 'tests': 'test', 'spec': 'test', 'specs': 'test',
  'types': 'types', 'interfaces': 'types', 'schemas': 'types', 'contracts': 'types', 'dtos': 'types',
  'hooks': 'hooks',
  'store': 'state', 'state': 'state', 'reducers': 'state', 'actions': 'state', 'slices': 'state',
  'assets': 'assets', 'static': 'assets', 'public': 'assets',
  'migrations': 'data',
  'management': 'config', 'commands': 'config',
  'templatetags': 'utility',
  'signals': 'service',
  'serializers': 'api',
  'cmd': 'entry',
  'internal': 'service',
  'pkg': 'utility',
  'dto': 'types', 'request': 'types', 'response': 'types',
  'entity': 'data',
  'controller': 'api',
  'routers': 'api',
  'composables': 'service',
  'blueprints': 'api',
  'mailers': 'service', 'jobs': 'service', 'channels': 'service',
  'bin': 'entry',
  'docs': 'documentation', 'documentation': 'documentation', 'wiki': 'documentation',
  'deploy': 'infrastructure', 'deployment': 'infrastructure', 'infra': 'infrastructure', 'infrastructure': 'infrastructure',
  'docker': 'infrastructure',
  'sql': 'data', 'database': 'data', 'schema': 'data',
  // Project-specific patterns
  'include': 'modules',  // This is too broad, handle specially
  'src': 'modules',
  'osoFile': 'config',
  'osoInclude': 'config',
  'qrc': 'assets',
  'DetachThread': 'service',  // Threads are service-layer infrastructure
  'NoHardWare': 'test',
  'func': 'service',  // Image processing functions = service logic
  'Modules': 'service',  // Module definitions
};

const patternMatches = {};
for (const group of Object.keys(directoryGroups)) {
  patternMatches[group] = dirPatterns[group] || null;
}

// Also check file-level patterns for flat structures
const fileLevelPatterns = {};

// ---- H. Deployment Topology Detection ----

const deploymentTopology = {
  hasDockerfile: false,
  hasCompose: false,
  hasK8s: false,
  hasTerraform: false,
  hasCI: false,
  infraFiles: []
};

// Check among all nodes
for (const node of fileNodes) {
  const name = node.name || '';
  const fp = node.filePath || '';

  if (/Dockerfile/i.test(name)) {
    deploymentTopology.hasDockerfile = true;
    deploymentTopology.infraFiles.push(fp);
  }
  if (/docker-compose/i.test(name)) {
    deploymentTopology.hasCompose = true;
    deploymentTopology.infraFiles.push(fp);
  }
  if (/\.tf\b|\.tfvars\b/i.test(name)) {
    deploymentTopology.hasTerraform = true;
    deploymentTopology.infraFiles.push(fp);
  }
  if (/k8s|kubernetes|helm/i.test(fp)) {
    deploymentTopology.hasK8s = true;
    deploymentTopology.infraFiles.push(fp);
  }
  if (/\.github\/workflows|\.gitlab-ci|Jenkinsfile|\.circleci/i.test(fp)) {
    deploymentTopology.hasCI = true;
    deploymentTopology.infraFiles.push(fp);
  }
  if (/CMakeLists\.txt$/i.test(name) || /\.cmake$/i.test(name)) {
    deploymentTopology.infraFiles.push(fp);
  }
}

// ---- I. Data Pipeline Detection ----

const dataPipeline = {
  schemaFiles: [],
  migrationFiles: [],
  dataModelFiles: [],
  apiHandlerFiles: []
};

for (const node of fileNodes) {
  const fp = node.filePath || '';
  const name = node.name || '';
  const tags = node.tags || [];

  if (/\.sql$/i.test(name)) dataPipeline.schemaFiles.push(node.id);
  if (/migration/i.test(fp) && /\.sql$/i.test(name)) dataPipeline.migrationFiles.push(node.id);
  if (/\.graphql$|\.gql$|\.proto$/i.test(name)) dataPipeline.schemaFiles.push(node.id);
  if (/model|entity|schema/i.test(fp) && (tags.includes('数据结构') || tags.includes('配置'))) {
    dataPipeline.dataModelFiles.push(node.id);
  }
  if (tags.includes('UI') || tags.includes('主窗口')) {
    dataPipeline.apiHandlerFiles.push(node.id);
  }
}

// DSL schema files
for (const node of fileNodes) {
  const fp = node.filePath || '';
  if (/osoFile/.test(fp) && /\.oso$/.test(fp)) {
    dataPipeline.schemaFiles.push(node.id);
  }
  if (/osoInclude/.test(fp)) {
    dataPipeline.dataModelFiles.push(node.id);
  }
}

// ---- J. Documentation Coverage ----

const groupsWithDocs = new Set();
for (const node of fileNodes) {
  const group = fileToGroup[node.id];
  if (!group) continue;
  const name = node.name || '';
  if (/\.md$|\.rst$/i.test(name) || name === 'README.md') {
    groupsWithDocs.add(group);
  }
  if (node.type === 'document') {
    groupsWithDocs.add(group);
  }
}

const totalGroups = Object.keys(directoryGroups).length;
const docCoverage = {
  groupsWithDocs: groupsWithDocs.size,
  totalGroups,
  coverageRatio: totalGroups > 0 ? groupsWithDocs.size / totalGroups : 0,
  undocumentedGroups: Object.keys(directoryGroups).filter(g => !groupsWithDocs.has(g))
};

// ---- K. Dependency Direction ----

const dependencyDirection = [];
for (const entry of interGroupImports) {
  const reverseKey = `${entry.to}->${entry.from}`;
  const reverseEntry = interGroupImports.find(e => `${e.from}->${e.to}` === reverseKey);
  const reverseCount = reverseEntry ? reverseEntry.count : 0;

  if (entry.count > reverseCount) {
    dependencyDirection.push({
      dependent: entry.from,
      dependsOn: entry.to
    });
  }
}

// ---- File Stats ----

const filesPerGroup = {};
for (const [group, ids] of Object.entries(directoryGroups)) {
  filesPerGroup[group] = ids.length;
}

const nodeTypeCounts = {};
for (const [type, ids] of Object.entries(nodeTypeGroups)) {
  nodeTypeCounts[type] = ids.length;
}

const fileStats = {
  totalFileNodes: fileNodes.length,
  filesPerGroup,
  nodeTypeCounts
};

// ---- Assemble output ----

const output = {
  scriptCompleted: true,
  directoryGroups,
  nodeTypeGroups,
  crossCategoryEdges,
  interGroupImports,
  intraGroupDensity,
  patternMatches,
  deploymentTopology,
  dataPipeline,
  docCoverage,
  dependencyDirection,
  fileStats,
  fileFanIn,
  fileFanOut
};

fs.writeFileSync(outputPath, JSON.stringify(output, null, 2), 'utf-8');
console.log(`Analysis complete. Output written to ${outputPath}`);
console.log(`Total file nodes: ${fileNodes.length}`);
console.log(`Directory groups: ${Object.keys(directoryGroups).length}`);
console.log(`Inter-group import pairs: ${interGroupImports.length}`);
process.exit(0);
