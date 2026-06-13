#!/usr/bin/env node
/**
 * Graph topology analysis script for tour-builder.
 * Analyzes fan-in, fan-out, entry points, BFS traversal, non-code files, clusters, and layers.
 */

const fs = require('fs');
const path = require('path');

// Read input and output paths from command line
const inputPath = process.argv[2];
const outputPath = process.argv[3];

if (!inputPath || !outputPath) {
  console.error('Usage: node ua-tour-analyze.js <input.json> <output.json>');
  process.exit(1);
}

let input;
try {
  input = JSON.parse(fs.readFileSync(inputPath, 'utf-8'));
} catch (e) {
  console.error('Failed to read or parse input file:', e.message);
  process.exit(1);
}

const { nodes, edges, layers } = input;

// Build node lookup
const nodeMap = {};
for (const node of nodes) {
  nodeMap[node.id] = node;
}

// Build adjacency maps
const fanIn = {};  // node id -> count of incoming edges
const fanOut = {}; // node id -> count of outgoing edges
const incomingEdges = {}; // node id -> set of source ids
const outgoingEdges = {}; // node id -> set of target ids
const allNodeIds = new Set();

for (const node of nodes) {
  fanIn[node.id] = 0;
  fanOut[node.id] = 0;
  incomingEdges[node.id] = new Set();
  outgoingEdges[node.id] = new Set();
  allNodeIds.add(node.id);
}

for (const edge of edges) {
  const src = edge.source;
  const tgt = edge.target;

  if (!allNodeIds.has(src) || !allNodeIds.has(tgt)) continue;

  fanIn[tgt] = (fanIn[tgt] || 0) + 1;
  fanOut[src] = (fanOut[src] || 0) + 1;

  if (!incomingEdges[tgt]) incomingEdges[tgt] = new Set();
  if (!outgoingEdges[src]) outgoingEdges[src] = new Set();

  incomingEdges[tgt].add(src);
  outgoingEdges[src].add(tgt);
}

// A. Fan-in ranking (importance)
const fanInRanking = Object.entries(fanIn)
  .map(([id, count]) => ({
    id,
    fanIn: count,
    name: nodeMap[id]?.name || id
  }))
  .sort((a, b) => b.fanIn - a.fanIn)
  .slice(0, 20);

// B. Fan-out ranking (scope)
const fanOutRanking = Object.entries(fanOut)
  .map(([id, count]) => ({
    id,
    fanOut: count,
    name: nodeMap[id]?.name || id
  }))
  .sort((a, b) => b.fanOut - a.fanOut)
  .slice(0, 20);

// C. Entry point candidates
const entryPointFileNames = [
  'index.ts', 'index.js', 'main.ts', 'main.js', 'app.ts', 'app.js',
  'server.ts', 'server.js', 'mod.rs', 'main.go', 'main.py', 'main.rs',
  'manage.py', 'app.py', 'wsgi.py', 'asgi.py', 'run.py', '__main__.py',
  'Application.java', 'Main.java', 'Program.cs', 'config.ru', 'index.php',
  'App.swift', 'Application.kt', 'main.cpp', 'main.c'
];

// Compute fan-in distribution for percentile thresholds
const fanInValues = Object.values(fanIn);
fanInValues.sort((a, b) => a - b);
const bottom25Thresh = fanInValues[Math.floor(fanInValues.length * 0.25)] || 0;
const top10Thresh = fanInValues[Math.ceil(fanInValues.length * 0.9) - 1] || 0;

// Compute fan-out distribution
const fanOutValues = Object.values(fanOut);
fanOutValues.sort((a, b) => a - b);
const top10FanOutThresh = fanOutValues[Math.ceil(fanOutValues.length * 0.9) - 1] || 0;

const entryPointCandidates = [];

for (const node of nodes) {
  let score = 0;
  const fileName = node.name || '';
  const filePath = node.filePath || '';

  if (node.type === 'file') {
    // Filename matches entry point pattern
    if (entryPointFileNames.includes(fileName)) {
      score += 3;
    }
    // File is at project root or one level deep
    const depth = filePath.split('/').filter(s => s && s !== '.').length;
    if (depth <= 2) {
      score += 1;
    }
    // High fan-out (top 10%)
    if (fanOut[node.id] >= top10FanOutThresh && fanOut[node.id] > 0) {
      score += 1;
    }
    // Low fan-in (bottom 25%)
    if (fanIn[node.id] <= bottom25Thresh) {
      score += 1;
    }
  }

  if (node.type === 'document') {
    if (fileName === 'README.md') {
      score += 5;
    } else if (fileName.endsWith('.md')) {
      score += 2;
    }
  }

  if (score > 0) {
    entryPointCandidates.push({
      id: node.id,
      score,
      name: node.name,
      summary: node.summary || '',
      type: node.type
    });
  }
}

entryPointCandidates.sort((a, b) => b.score - a.score);
const top5Candidates = entryPointCandidates.slice(0, 5);

// D. BFS Traversal from top code entry point
// Find the top code entry point (skip document nodes)
const topCodeEntry = entryPointCandidates.find(c => c.type === 'file') || top5Candidates[0];

let bfsTraversal = {
  startNode: null,
  order: [],
  depthMap: {},
  byDepth: {}
};

if (topCodeEntry) {
  bfsTraversal.startNode = topCodeEntry.id;
  const visited = new Set();
  const queue = [{ id: topCodeEntry.id, depth: 0 }];
  visited.add(topCodeEntry.id);

  while (queue.length > 0) {
    const { id, depth } = queue.shift();
    bfsTraversal.order.push(id);
    bfsTraversal.depthMap[id] = depth;

    if (!bfsTraversal.byDepth[depth]) {
      bfsTraversal.byDepth[depth] = [];
    }
    bfsTraversal.byDepth[depth].push(id);

    // Follow imports and calls edges forward
    const neighbors = outgoingEdges[id] || new Set();
    for (const nextId of neighbors) {
      // Only follow imports and calls edges to code files
      // Check edge type
      const relevantEdges = edges.filter(e =>
        e.source === id && e.target === nextId &&
        (e.type === 'imports' || e.type === 'calls')
      );
      if (relevantEdges.length > 0 && !visited.has(nextId)) {
        visited.add(nextId);
        queue.push({ id: nextId, depth: depth + 1 });
      }
    }
  }
}

// E. Non-code file inventory
const nonCodeFiles = {
  documentation: [],
  infrastructure: [],
  data: [],
  config: []
};

for (const node of nodes) {
  const entry = {
    id: node.id,
    name: node.name,
    summary: node.summary || '',
    type: node.type
  };

  switch (node.type) {
    case 'document':
      nonCodeFiles.documentation.push(entry);
      break;
    case 'service':
    case 'pipeline':
    case 'resource':
      nonCodeFiles.infrastructure.push(entry);
      break;
    case 'table':
    case 'schema':
    case 'endpoint':
      nonCodeFiles.data.push(entry);
      break;
    case 'config':
      nonCodeFiles.config.push(entry);
      break;
  }
}

// F. Tightly coupled clusters
const clusters = [];

// Find bidirectional pairs
const bidirectionalPairs = new Set();
const clusterNodes = new Set();

for (const edge of edges) {
  const src = edge.source;
  const tgt = edge.target;
  if (!allNodeIds.has(src) || !allNodeIds.has(tgt)) continue;

  // Check for reverse edge
  const hasReverse = edges.some(e =>
    e.source === tgt && e.target === src &&
    ((e.type === 'imports' && edge.type === 'imports') ||
     (e.type === 'calls' && edge.type === 'calls'))
  );

  if (hasReverse) {
    const pairKey = [src, tgt].sort().join('|||');
    if (!bidirectionalPairs.has(pairKey)) {
      bidirectionalPairs.add(pairKey);
      clusterNodes.add(src);
      clusterNodes.add(tgt);
    }
  }
}

// Expand clusters: add nodes that connect to 2+ existing cluster members
const expandedClusters = [];
const clusterMembers = new Set(clusterNodes);

// Try to grow clusters from bidirectional pairs
const pairList = Array.from(bidirectionalPairs).map(p => p.split('|||'));
const usedInCluster = new Set();

for (const pair of pairList) {
  if (usedInCluster.has(pair[0]) && usedInCluster.has(pair[1])) continue;

  const cluster = new Set(pair);
  let changed = true;

  while (changed) {
    changed = false;
    for (const nodeId of allNodeIds) {
      if (cluster.has(nodeId)) continue;
      // Count connections to cluster
      let connectionCount = 0;
      for (const member of cluster) {
        const hasEdge1 = edges.some(e => e.source === nodeId && e.target === member);
        const hasEdge2 = edges.some(e => e.target === nodeId && e.source === member);
        if (hasEdge1 || hasEdge2) connectionCount++;
        if (connectionCount >= 2) break;
      }
      if (connectionCount >= 2) {
        cluster.add(nodeId);
        changed = true;
      }
    }
  }

  // Only emit clusters of 2-5 nodes
  if (cluster.size >= 2 && cluster.size <= 5) {
    const clusterNodesArr = Array.from(cluster);
    // Count edges within cluster
    let edgeCount = 0;
    for (let i = 0; i < clusterNodesArr.length; i++) {
      for (let j = i + 1; j < clusterNodesArr.length; j++) {
        const e1 = edges.filter(e => e.source === clusterNodesArr[i] && e.target === clusterNodesArr[j]).length;
        const e2 = edges.filter(e => e.source === clusterNodesArr[j] && e.target === clusterNodesArr[i]).length;
        edgeCount += e1 + e2;
      }
    }
    expandedClusters.push({ nodes: clusterNodesArr, edgeCount });
    for (const n of clusterNodesArr) usedInCluster.add(n);
  }
}

expandedClusters.sort((a, b) => b.edgeCount - a.edgeCount);
const topClusters = expandedClusters.slice(0, 10);

// G. Layer list
const layerOutput = {
  count: layers ? layers.length : 0,
  list: layers ? layers.map(l => ({ id: l.id, name: l.name, description: l.description })) : []
};

// H. Node summary index
const nodeSummaryIndex = {};
for (const node of nodes) {
  nodeSummaryIndex[node.id] = {
    name: node.name,
    type: node.type,
    summary: node.summary || ''
  };
}

// Build output
const output = {
  scriptCompleted: true,
  entryPointCandidates: top5Candidates,
  fanInRanking,
  fanOutRanking,
  bfsTraversal,
  nonCodeFiles,
  clusters: topClusters,
  layers: layerOutput,
  nodeSummaryIndex,
  totalNodes: nodes.length,
  totalEdges: edges.length
};

// Write output
try {
  fs.writeFileSync(outputPath, JSON.stringify(output, null, 2), 'utf-8');
} catch (e) {
  console.error('Failed to write output file:', e.message);
  process.exit(1);
}

console.log('Analysis complete. Output written to:', outputPath);
console.log(`Total nodes: ${nodes.length}, Total edges: ${edges.length}`);
console.log(`Entry point candidates: ${top5Candidates.length}`);
console.log(`BFS nodes reached: ${bfsTraversal.order.length}`);
console.log(`Non-code files: docs=${nonCodeFiles.documentation.length}, infra=${nonCodeFiles.infrastructure.length}, data=${nonCodeFiles.data.length}, config=${nonCodeFiles.config.length}`);
console.log(`Clusters found: ${topClusters.length}`);
console.log(`Layers: ${layerOutput.count}`);
