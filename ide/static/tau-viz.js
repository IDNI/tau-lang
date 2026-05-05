/**
 * Tau temporal formula visualization — Release 5.0
 *
 * Provides:
 *  1. Semantics diagram — truth-value table showing T/F at each time step
 *     for each subformula, with operator-specific example scenarios
 *  2. AST tree rendering — interactive parse tree with subformula text
 *  3. Operator legend — color-coded reference with formal semantics
 */
const TauViz = {

  /* ---- Color scheme (Catppuccin Mocha) ---- */
  colors: {
    G:     '#cba6f7',  // always — mauve
    F:     '#f38ba8',  // eventually — red
    U:     '#fab387',  // until — peach
    R:     '#a6e3a1',  // release — green
    W:     '#89b4fa',  // weak until — blue
    S:     '#f9e2af',  // since — yellow
    T:     '#94e2d5',  // triggered — teal
    A:     '#f5c2e7',  // for-all paths — pink
    E:     '#74c7ec',  // exists path — sapphire
    and:   '#89dceb',  // conjunction
    or:    '#89dceb',
    neg:   '#eba0ac',  // negation — maroon
    sem:   '#f38ba8',  // semantic neg
    imp:   '#b4befe',  // implication — lavender
    atom:  '#cdd6f4',  // atoms — text
    bg:    '#1e1e2e',
    bgRow: '#181825',
    grid:  '#313244',
    text:  '#cdd6f4',
    true_:  '#a6e3a1', // green for T
    false_: '#f38ba8', // red for F
    maybe:  '#f9e2af', // yellow for unknown/either
    muted:  '#6c7086', // overlay0 for muted text
  },

  /* ---- Pre-processing ---- */

  _preprocess(formula) {
    const lines = formula
      .split('\n')
      .map(line => line.replace(/(--|#).*$/, '').trim())
      .filter(line => line.length > 0)
      .join(' ')
      .replace(/\s+/g, ' ')
      .trim();
    const statements = lines.split('.').map(s => s.trim()).filter(s => s.length > 0);
    const declPattern = /^\w+\s*(?::\s*\w+)?\s*:=\s*(in|out)\s+.+$/;
    const recRelPattern = /^\w+(?:\[\w+\])?\s*\(.*\)\s*(?::\s*\w+)?\s*:=/;
    const formulaStatements = statements.filter(s => !declPattern.test(s) && !recRelPattern.test(s));
    return formulaStatements.map(s => formulaStatements.length > 1 ? `(${s})` : s).join(' && ').trim();
  },

  /* ---- Parser ---- */

  parse(formula) {
    const src = this._preprocess(formula);
    if (!src) return { type: 'atom', value: '(empty)' };
    try {
      return this._parseExpr(src);
    } catch (_) {
      return { type: 'atom', value: src };
    }
  },

  _parseExpr(src) {
    src = src.trim();
    if (!src) return { type: 'atom', value: '' };

    // Binary operators by precedence (lowest first)
    const binGroups = [
      ['<->'],
      ['->', '<-'],
      ['||'],
      ['&&'],
      ['^^'],
      ['U', 'R', 'W', 'S', 'T'],
    ];

    for (const ops of binGroups) {
      const idx = this._findTopLevel(src, ops);
      if (idx !== null) {
        const { op, pos } = idx;
        const left = src.slice(0, pos).trim();
        const right = src.slice(pos + op.length).trim();
        return {
          type: 'binary', op,
          left: this._parseExpr(left),
          right: this._parseExpr(right),
        };
      }
    }

    if (src.startsWith('[]')) {
      return { type: 'unary', op: 'G', child: this._parseExpr(src.slice(2).trim()) };
    }
    if (src.startsWith('<>')) {
      return { type: 'unary', op: 'F', child: this._parseExpr(src.slice(2).trim()) };
    }

    for (const op of ['always', 'sometimes', 'G', 'F', 'A', 'E', '!']) {
      if (src.startsWith(op + ' ') || src.startsWith(op + '(')) {
        return {
          type: 'unary', op,
          child: this._parseExpr(src.slice(op.length).trim()),
        };
      }
    }
    if (src.startsWith('- ') || (src.startsWith('-') && src[1] === '(')) {
      return { type: 'unary', op: '-', child: this._parseExpr(src.slice(1).trim()) };
    }

    for (const op of ['!=', '!<=', '!>=', '!<', '!>', '<=', '>=', '=', '<', '>']) {
      const pos = this._findTopLevelSingle(src, op);
      if (pos >= 0) {
        return {
          type: 'relation', op,
          left: { type: 'atom', value: src.slice(0, pos).trim() },
          right: { type: 'atom', value: src.slice(pos + op.length).trim() },
        };
      }
    }

    if (src.startsWith('(') && this._matchingParen(src, 0) === src.length - 1) {
      return this._parseExpr(src.slice(1, -1));
    }
    if (src.startsWith('(')) {
      const close = this._matchingParen(src, 0);
      if (close > 0 && close < src.length - 1) {
        const rest = src.slice(close + 1).trim();
        for (const ops of [['<->'], ['->', '<-'], ['||'], ['&&'], ['^^'], ['U', 'R', 'W', 'S', 'T']]) {
          for (const op of ops) {
            if (rest.startsWith(op + ' ') || rest.startsWith(op + '(') || rest === op) {
              return {
                type: 'binary', op,
                left: this._parseExpr(src.slice(1, close)),
                right: this._parseExpr(rest.slice(op.length).trim()),
              };
            }
          }
        }
      }
    }

    return { type: 'atom', value: src };
  },

  _findTopLevel(src, ops) {
    let depth = 0;
    let best = null;
    for (let i = 0; i < src.length; i++) {
      if (src[i] === '(') { depth++; continue; }
      if (src[i] === ')') { depth--; continue; }
      if (depth !== 0) continue;
      for (const op of ops) {
        if (i + op.length > src.length) continue;
        if (src.slice(i, i + op.length) !== op) continue;
        if (op.length === 1 && /[a-zA-Z0-9_]/.test(op)) {
          const before = i > 0 ? src[i - 1] : ' ';
          const after = i + op.length < src.length ? src[i + op.length] : ' ';
          if (/[a-zA-Z0-9_]/.test(before) || /[a-zA-Z0-9_]/.test(after)) continue;
        }
        if (op.length > 1 && /^[a-z]+$/i.test(op)) {
          const before = i > 0 ? src[i - 1] : ' ';
          const after = i + op.length < src.length ? src[i + op.length] : ' ';
          if (/[a-zA-Z0-9_]/.test(before) || /[a-zA-Z0-9_]/.test(after)) continue;
        }
        best = { op, pos: i };
      }
    }
    return best;
  },

  _findTopLevelSingle(src, op) {
    let depth = 0;
    for (let i = 0; i <= src.length - op.length; i++) {
      if (src[i] === '(') { depth++; continue; }
      if (src[i] === ')') { depth--; continue; }
      if (depth !== 0) continue;
      if (src.slice(i, i + op.length) === op) {
        if (op === '=' && i > 0 && (src[i - 1] === '!' || src[i - 1] === '<' || src[i - 1] === '>')) continue;
        if (op === '<' && i + 1 < src.length && (src[i + 1] === '-' || src[i + 1] === '=' || src[i + 1] === '>')) continue;
        if (op === '>' && i > 0 && (src[i - 1] === '-' || src[i - 1] === '=' || src[i - 1] === '<')) continue;
        return i;
      }
    }
    return -1;
  },

  _matchingParen(src, openIdx) {
    let depth = 0;
    for (let i = openIdx; i < src.length; i++) {
      if (src[i] === '(') depth++;
      else if (src[i] === ')') { depth--; if (depth === 0) return i; }
    }
    return -1;
  },

  /* ---- Node to string ---- */

  _nodeToString(node) {
    if (!node) return '';
    if (node.type === 'atom') return node.value;
    if (node.type === 'unary') {
      const c = this._nodeToString(node.child);
      const canonical = node.op === 'always' ? 'G' : node.op === 'sometimes' ? 'F' : node.op;
      return `${canonical}(${c})`;
    }
    if (node.type === 'binary') {
      const l = this._nodeToString(node.left);
      const r = this._nodeToString(node.right);
      return `(${l} ${node.op} ${r})`;
    }
    if (node.type === 'relation') {
      return `${this._nodeToString(node.left)} ${node.op} ${this._nodeToString(node.right)}`;
    }
    return '';
  },

  _shortLabel(node, maxLen = 40) {
    const s = this._nodeToString(node);
    return s.length > maxLen ? s.slice(0, maxLen - 1) + '\u2026' : s;
  },

  /* ==================================================================== */
  /*  SEMANTICS DIAGRAM — truth-value table for temporal operators         */
  /* ==================================================================== */

  renderTimeline(formula, steps = 10) {
    const ast = this.parse(formula);
    const layers = this._decomposeFormula(ast);
    if (layers.length === 0) {
      return '<div class="viz-empty">No temporal structure found.</div>';
    }

    // Generate example truth values for each layer
    const scenarios = this._generateScenarios(layers, steps);

    // Build SVG
    const W = 740, padLeft = 200, padTop = 44, padRight = 20;
    const stepW = (W - padLeft - padRight) / steps;
    const rowH = 36;
    const H = padTop + layers.length * rowH + 24;

    let svg = `<svg width="${W}" height="${H}" xmlns="http://www.w3.org/2000/svg" style="font-family:'JetBrains Mono',monospace;font-size:11px;">`;
    svg += `<rect width="${W}" height="${H}" fill="${this.colors.bg}" rx="6"/>`;

    // Column headers (time steps)
    for (let t = 0; t < steps; t++) {
      const x = padLeft + t * stepW + stepW / 2;
      svg += `<text x="${x}" y="${padTop - 10}" fill="${this.colors.text}" text-anchor="middle" font-size="10" opacity="0.6">t=${t}</text>`;
      svg += `<line x1="${x}" y1="${padTop - 4}" x2="${x}" y2="${H - 12}" stroke="${this.colors.grid}" stroke-width="0.5" stroke-dasharray="2,3"/>`;
    }

    // Row for each formula layer
    layers.forEach((layer, idx) => {
      const y = padTop + idx * rowH;
      const vals = scenarios[idx];
      const color = this._colorForNode(layer.node);

      // Alternating row background
      if (idx % 2 === 0) {
        svg += `<rect x="0" y="${y}" width="${W}" height="${rowH}" fill="${this.colors.bgRow}" opacity="0.5"/>`;
      }

      // Row label: operator badge + subformula text
      const opName = layer.opName || '';
      const labelText = this._escSvg(layer.label);
      if (opName) {
        const badgeW = opName.length * 7 + 8;
        svg += `<rect x="4" y="${y + 8}" width="${badgeW}" height="18" fill="${color}" opacity="0.15" rx="3"/>`;
        svg += `<text x="${8}" y="${y + 21}" fill="${color}" font-size="10" font-weight="700">${opName}</text>`;
        svg += `<text x="${badgeW + 10}" y="${y + 21}" fill="${this.colors.text}" font-size="10" opacity="0.85">${labelText}</text>`;
      } else {
        svg += `<text x="8" y="${y + 21}" fill="${this.colors.text}" font-size="10" opacity="0.85">${labelText}</text>`;
      }

      // Truth values at each step
      for (let t = 0; t < steps; t++) {
        const cx = padLeft + t * stepW + stepW / 2;
        const cy = y + rowH / 2 + 1;
        const v = vals[t]; // 'T', 'F', or '?'

        if (v === 'T') {
          svg += `<rect x="${cx - stepW/2 + 1}" y="${y + 2}" width="${stepW - 2}" height="${rowH - 4}" fill="${this.colors.true_}" opacity="0.12" rx="2"/>`;
          svg += `<text x="${cx}" y="${cy + 4}" fill="${this.colors.true_}" text-anchor="middle" font-size="12" font-weight="700">T</text>`;
        } else if (v === 'F') {
          svg += `<rect x="${cx - stepW/2 + 1}" y="${y + 2}" width="${stepW - 2}" height="${rowH - 4}" fill="${this.colors.false_}" opacity="0.08" rx="2"/>`;
          svg += `<text x="${cx}" y="${cy + 4}" fill="${this.colors.false_}" text-anchor="middle" font-size="12" font-weight="600" opacity="0.7">F</text>`;
        } else {
          svg += `<text x="${cx}" y="${cy + 4}" fill="${this.colors.maybe}" text-anchor="middle" font-size="11" opacity="0.5">?</text>`;
        }
      }
    });

    // Bottom: scenario description
    svg += `<text x="${W/2}" y="${H - 4}" fill="${this.colors.muted}" text-anchor="middle" font-size="9">Example scenario showing one possible trace. Atoms shown as given; operators derived.</text>`;
    svg += '</svg>';
    return svg;
  },

  /**
   * Decompose a formula into layers for the truth table.
   * Each layer = { node, label, opName, depth }.
   * Bottom-up: atoms first, then operators.
   */
  _decomposeFormula(node, depth = 0) {
    if (!node) return [];
    const layers = [];

    if (node.type === 'atom') {
      layers.push({ node, label: this._shortLabel(node, 30), opName: '', depth });
      return layers;
    }

    if (node.type === 'relation') {
      layers.push({ node, label: this._shortLabel(node, 30), opName: node.op, depth });
      return layers;
    }

    if (node.type === 'unary') {
      const canonical = node.op === 'always' ? 'G' : node.op === 'sometimes' ? 'F' : node.op;
      const childLayers = this._decomposeFormula(node.child, depth + 1);
      layers.push(...childLayers);
      layers.push({ node, label: this._shortLabel(node, 30), opName: canonical, depth });
      return layers;
    }

    if (node.type === 'binary') {
      const leftLayers = this._decomposeFormula(node.left, depth + 1);
      const rightLayers = this._decomposeFormula(node.right, depth + 1);
      layers.push(...leftLayers);
      layers.push(...rightLayers);
      layers.push({ node, label: this._shortLabel(node, 30), opName: node.op, depth });
      return layers;
    }

    return layers;
  },

  /**
   * Generate example truth values for each layer over `steps` time steps.
   * Returns array of arrays: scenarios[layerIdx][step] = 'T'|'F'|'?'
   *
   * Strategy: assign concrete truth values to atoms, then derive operator
   * values using correct LTL semantics.
   */
  _generateScenarios(layers, steps) {
    const results = [];
    const nodeVals = new Map(); // node reference → truth values array

    for (const layer of layers) {
      const vals = this._evalNode(layer.node, steps, nodeVals);
      nodeVals.set(layer.node, vals);
      results.push(vals);
    }
    return results;
  },

  /**
   * Compute truth values for a node over `steps` time steps.
   * For atoms: generate a plausible pattern.
   * For operators: compute from children using correct LTL semantics.
   */
  _evalNode(node, steps, cache) {
    if (cache.has(node)) return cache.get(node);

    const arr = new Array(steps).fill('?');

    if (node.type === 'atom' || node.type === 'relation') {
      // Generate a semi-realistic pattern for atoms
      const s = this._nodeToString(node);
      const hash = this._hashStr(s);
      // Use hash to pick a pattern style
      const style = hash % 5;
      for (let t = 0; t < steps; t++) {
        switch (style) {
          case 0: arr[t] = 'T'; break;                          // always true
          case 1: arr[t] = t >= 3 ? 'T' : 'F'; break;          // becomes true at t=3
          case 2: arr[t] = t % 2 === 0 ? 'T' : 'F'; break;     // alternating
          case 3: arr[t] = t < steps - 2 ? 'T' : 'F'; break;   // true then false
          case 4: arr[t] = t === 0 ? 'F' : 'T'; break;         // false at start
        }
      }
      cache.set(node, arr);
      return arr;
    }

    if (node.type === 'unary') {
      const child = this._evalNode(node.child, steps, cache);
      const canonical = node.op === 'always' ? 'G' : node.op === 'sometimes' ? 'F' : node.op;

      switch (canonical) {
        case 'G': // G(φ): true at t iff φ is true at all t' >= t
          for (let t = 0; t < steps; t++) {
            let allTrue = true;
            for (let t2 = t; t2 < steps; t2++) {
              if (child[t2] !== 'T') { allTrue = false; break; }
            }
            arr[t] = allTrue ? 'T' : 'F';
          }
          break;

        case 'F': // F(φ): true at t iff φ is true at some t' >= t
          for (let t = 0; t < steps; t++) {
            let someTrue = false;
            for (let t2 = t; t2 < steps; t2++) {
              if (child[t2] === 'T') { someTrue = true; break; }
            }
            arr[t] = someTrue ? 'T' : 'F';
          }
          break;

        case '!': // negation
          for (let t = 0; t < steps; t++) {
            arr[t] = child[t] === 'T' ? 'F' : child[t] === 'F' ? 'T' : '?';
          }
          break;

        case '-': // semantic negation (same truth table as !)
          for (let t = 0; t < steps; t++) {
            arr[t] = child[t] === 'T' ? 'F' : child[t] === 'F' ? 'T' : '?';
          }
          break;

        case 'A': // for-all paths — in single-trace view, same as the child
        case 'E': // exists-path — in single-trace view, same as the child
          for (let t = 0; t < steps; t++) arr[t] = child[t];
          break;

        default:
          for (let t = 0; t < steps; t++) arr[t] = child[t];
      }

      cache.set(node, arr);
      return arr;
    }

    if (node.type === 'binary') {
      const left = this._evalNode(node.left, steps, cache);
      const right = this._evalNode(node.right, steps, cache);

      switch (node.op) {
        case '&&': // conjunction
          for (let t = 0; t < steps; t++) {
            arr[t] = (left[t] === 'T' && right[t] === 'T') ? 'T' : 'F';
          }
          break;

        case '||': // disjunction
          for (let t = 0; t < steps; t++) {
            arr[t] = (left[t] === 'T' || right[t] === 'T') ? 'T' : 'F';
          }
          break;

        case '^^': // exclusive or
          for (let t = 0; t < steps; t++) {
            arr[t] = (left[t] !== right[t]) ? 'T' : 'F';
          }
          break;

        case '->': // implication: ¬φ ∨ ψ
          for (let t = 0; t < steps; t++) {
            arr[t] = (left[t] !== 'T' || right[t] === 'T') ? 'T' : 'F';
          }
          break;

        case '<-': // reverse implication: φ ∨ ¬ψ
          for (let t = 0; t < steps; t++) {
            arr[t] = (right[t] !== 'T' || left[t] === 'T') ? 'T' : 'F';
          }
          break;

        case '<->': // equivalence
          for (let t = 0; t < steps; t++) {
            arr[t] = (left[t] === right[t]) ? 'T' : 'F';
          }
          break;

        case 'U': // Until: φ U ψ is true at t iff ∃k≥t: ψ[k] ∧ ∀j∈[t,k): φ[j]
          for (let t = 0; t < steps; t++) {
            let found = false;
            for (let k = t; k < steps; k++) {
              if (right[k] === 'T') {
                let phiHolds = true;
                for (let j = t; j < k; j++) {
                  if (left[j] !== 'T') { phiHolds = false; break; }
                }
                if (phiHolds) { found = true; break; }
              }
            }
            arr[t] = found ? 'T' : 'F';
          }
          break;

        case 'R': // Release: φ R ψ ≡ ¬(¬φ U ¬ψ)
          // ψ must hold at all steps until (and including) when φ first holds
          // If φ never holds, ψ must hold forever
          for (let t = 0; t < steps; t++) {
            // Check ¬(¬φ U ¬ψ) at t
            let negUntil = false; // is ¬φ U ¬ψ true at t?
            for (let k = t; k < steps; k++) {
              if (right[k] !== 'T') { // ¬ψ[k] true (i.e., ψ[k] false)
                let negPhiHolds = true;
                for (let j = t; j < k; j++) {
                  if (left[j] === 'T') { negPhiHolds = false; break; } // ¬φ[j] false
                }
                if (negPhiHolds) { negUntil = true; break; }
              }
            }
            arr[t] = negUntil ? 'F' : 'T';
          }
          break;

        case 'W': // Weak Until: φ W ψ ≡ (φ U ψ) ∨ G(φ)
          for (let t = 0; t < steps; t++) {
            // Check φ U ψ at t
            let untilHolds = false;
            for (let k = t; k < steps; k++) {
              if (right[k] === 'T') {
                let phiHolds = true;
                for (let j = t; j < k; j++) {
                  if (left[j] !== 'T') { phiHolds = false; break; }
                }
                if (phiHolds) { untilHolds = true; break; }
              }
            }
            // Check G(φ) at t
            let gHolds = true;
            for (let t2 = t; t2 < steps; t2++) {
              if (left[t2] !== 'T') { gHolds = false; break; }
            }
            arr[t] = (untilHolds || gHolds) ? 'T' : 'F';
          }
          break;

        case 'S': // Since (past): φ S ψ at t iff ∃k≤t: ψ[k] ∧ ∀j∈(k,t]: φ[j]
          for (let t = 0; t < steps; t++) {
            let found = false;
            for (let k = t; k >= 0; k--) {
              if (right[k] === 'T') {
                let phiHolds = true;
                for (let j = k + 1; j <= t; j++) {
                  if (left[j] !== 'T') { phiHolds = false; break; }
                }
                if (phiHolds) { found = true; break; }
              }
            }
            arr[t] = found ? 'T' : 'F';
          }
          break;

        case 'T': // Triggered (past): φ T ψ ≡ ¬(¬φ S ¬ψ)
          for (let t = 0; t < steps; t++) {
            let negSince = false;
            for (let k = t; k >= 0; k--) {
              if (right[k] !== 'T') { // ¬ψ[k]
                let negPhiHolds = true;
                for (let j = k + 1; j <= t; j++) {
                  if (left[j] === 'T') { negPhiHolds = false; break; }
                }
                if (negPhiHolds) { negSince = true; break; }
              }
            }
            arr[t] = negSince ? 'F' : 'T';
          }
          break;

        default:
          for (let t = 0; t < steps; t++) arr[t] = '?';
      }

      cache.set(node, arr);
      return arr;
    }

    cache.set(node, arr);
    return arr;
  },

  /** Simple string hash for deterministic but varied atom patterns. */
  _hashStr(s) {
    let h = 0;
    for (let i = 0; i < s.length; i++) {
      h = ((h << 5) - h + s.charCodeAt(i)) | 0;
    }
    return Math.abs(h);
  },

  /** Get the operator-appropriate color for a node. */
  _colorForNode(node) {
    if (!node) return this.colors.atom;
    if (node.type === 'atom' || node.type === 'relation') return this.colors.atom;
    if (node.type === 'unary') {
      const canonical = node.op === 'always' ? 'G' : node.op === 'sometimes' ? 'F' : node.op;
      return this.colors[canonical] || this.colors.neg;
    }
    if (node.type === 'binary') {
      return this.colors[node.op] || this.colors.and;
    }
    return this.colors.atom;
  },

  /* ==================================================================== */
  /*  AST TREE — with subformula text at each node                        */
  /* ==================================================================== */

  renderAST(formula) {
    const ast = this.parse(formula);
    return '<div class="ast-tree">' + this._renderNode(ast, 0, true) + '</div>';
  },

  _renderNode(node, depth, isLast) {
    if (!node) return '';
    const indent = depth * 24;

    const connector = depth > 0
      ? `<span class="ast-connector" style="width:${indent}px">${isLast ? '\u2514' : '\u251c'}\u2500 </span>`
      : '';

    if (node.type === 'atom') {
      return `<div class="ast-node">
        ${connector}<span class="ast-badge atom">atom</span>
        <code>${this._escHtml(node.value)}</code>
      </div>`;
    }

    if (node.type === 'unary') {
      const canonical = node.op === 'always' ? 'G' : node.op === 'sometimes' ? 'F' : node.op;
      const color = this.colors[canonical] || this.colors.neg;
      return `<div class="ast-node">
        ${connector}<span class="ast-badge" style="background:${color}22;color:${color};border:1px solid ${color}44">${this._escHtml(canonical)}</span>
        <span class="ast-label">${this._opDescription(node.op)}</span>
        <code class="ast-formula">${this._escHtml(this._shortLabel(node, 50))}</code>
      </div>` + this._renderNode(node.child, depth + 1, true);
    }

    if (node.type === 'binary') {
      const color = this.colors[node.op] || this.colors.and;
      let html = '';
      html += `<div class="ast-node">
        ${connector}<span class="ast-badge" style="background:${color}22;color:${color};border:1px solid ${color}44">${this._escHtml(node.op)}</span>
        <span class="ast-label">${this._opDescription(node.op)}</span>
        <code class="ast-formula">${this._escHtml(this._shortLabel(node, 50))}</code>
      </div>`;
      html += this._renderNode(node.left, depth + 1, false);
      html += this._renderNode(node.right, depth + 1, true);
      return html;
    }

    if (node.type === 'relation') {
      let html = '';
      html += `<div class="ast-node">
        ${connector}<span class="ast-badge" style="background:#89dceb22;color:#89dceb;border:1px solid #89dceb44">${this._escHtml(node.op)}</span>
        <span class="ast-label">comparison</span>
        <code class="ast-formula">${this._escHtml(this._shortLabel(node, 50))}</code>
      </div>`;
      html += this._renderNode(node.left, depth + 1, false);
      html += this._renderNode(node.right, depth + 1, true);
      return html;
    }

    return '';
  },

  _opDescription(op) {
    const descs = {
      'G': 'globally (always)', 'F': 'eventually (future)',
      'always': 'globally', 'sometimes': 'eventually',
      'U': 'until', 'R': 'release', 'W': 'weak until',
      'S': 'since (past)', 'T': 'triggered (past)',
      'A': 'for-all paths (CTL*)', 'E': 'exists-path (CTL*)',
      '!': 'negation', '-': 'semantic negation',
      '&&': 'conjunction', '||': 'disjunction', '^^': 'exclusive or',
      '->': 'implication', '<-': 'reverse implication', '<->': 'equivalence',
    };
    return descs[op] || op;
  },

  /* ==================================================================== */
  /*  OPERATOR LEGEND — with formal semantics                             */
  /* ==================================================================== */

  renderLegend() {
    const groups = [
      { title: 'Future Temporal Operators', ops: [
        ['G / always / []', 'G(\u03c6) \u2261 \u03c6 holds at every step t\u2019 \u2265 t', this.colors.G],
        ['F / sometimes / <>', 'F(\u03c6) \u2261 \u03c6 holds at some step t\u2019 \u2265 t', this.colors.F],
        ['U', '\u03c6 U \u03c8 \u2261 \u2203k\u2265t: \u03c8(k) \u2227 \u2200j\u2208[t,k): \u03c6(j)', this.colors.U],
        ['R', '\u03c6 R \u03c8 \u2261 \u00ac(\u00ac\u03c6 U \u00ac\u03c8) \u2014 \u03c8 holds until \u03c6 releases it', this.colors.R],
        ['W', '\u03c6 W \u03c8 \u2261 (\u03c6 U \u03c8) \u2228 G(\u03c6) \u2014 like U but \u03c6 may hold forever', this.colors.W],
      ]},
      { title: 'Past Temporal Operators', ops: [
        ['S', '\u03c6 S \u03c8 \u2261 \u2203k\u2264t: \u03c8(k) \u2227 \u2200j\u2208(k,t]: \u03c6(j)', this.colors.S],
        ['T', '\u03c6 T \u03c8 \u2261 \u00ac(\u00ac\u03c6 S \u00ac\u03c8) \u2014 past dual of Release', this.colors.T],
      ]},
      { title: 'CTL* Path Quantifiers', ops: [
        ['A', 'A(\u03c6) \u2261 \u03c6 holds on all computation paths', this.colors.A],
        ['E', 'E(\u03c6) \u2261 \u03c6 holds on some computation path', this.colors.E],
        ['\u2212', 'Semantic negation \u2014 no winning strategy exists', this.colors.sem],
      ]},
      { title: 'Logical Connectives', ops: [
        ['&&', 'Conjunction \u2014 both must hold', this.colors.and],
        ['||', 'Disjunction \u2014 at least one holds', this.colors.or],
        ['!', 'Negation \u2014 inverts truth value', this.colors.neg],
        ['->', 'Implication \u2014 if \u03c6 then \u03c8', this.colors.imp],
        ['<->', 'Equivalence \u2014 both or neither', this.colors.imp],
      ]},
    ];

    let html = '<div class="viz-legend-grouped">';
    for (const group of groups) {
      html += `<div class="legend-group">`;
      html += `<div class="legend-group-title">${group.title}</div>`;
      html += `<div class="legend-group-items">`;
      for (const [name, desc, color] of group.ops) {
        html += `<div class="legend-item">
          <span class="legend-swatch" style="background:${color}"></span>
          <span class="legend-name" style="color:${color}">${name}</span>
          <span class="legend-desc">${desc}</span>
        </div>`;
      }
      html += `</div></div>`;
    }
    html += '</div>';
    return html;
  },

  /* ==================================================================== */
  /*  FULL VISUALIZATION                                                  */
  /* ==================================================================== */

  renderVisualization(formula) {
    if (!formula || !formula.trim()) {
      return `<div class="viz-empty">
        <p>Enter a temporal formula in the editor and click <strong>Visualize</strong> or press <strong>Ctrl+Shift+V</strong>.</p>
        <div class="viz-examples">
          <div class="viz-example-title">Try these examples:</div>
          <code>G(o1[t] = i1[t]).</code>
          <code>F(o1[t] = 1).</code>
          <code>(o1[t] = 0) U (o1[t] = 1).</code>
          <code>G(o1[t] = 0) && F(o1[t] = 1).</code>
          <code>(o1[t] = 0) W (i1[t] = 1).</code>
          <code>(o1[t] = 1) S (i1[t] = 0).</code>
        </div>
      </div>`;
    }

    let html = '<div class="viz-container">';

    // Preprocessed formula display
    const processed = this._preprocess(formula);
    html += `<div class="viz-formula-bar"><span class="viz-formula-label">Formula:</span> <code>${this._escHtml(processed)}</code></div>`;

    // Semantics diagram (truth table over time)
    html += '<h3 class="viz-heading">Semantics — Truth Values Over Time</h3>';
    html += '<p class="viz-subtitle">Each row shows a subformula. Values are computed bottom-up using correct LTL semantics on an example trace.</p>';
    html += '<div class="viz-timeline">' + this.renderTimeline(formula) + '</div>';

    // AST
    html += '<h3 class="viz-heading">Formula Structure (AST)</h3>';
    html += '<div class="viz-ast">' + this.renderAST(formula) + '</div>';

    // Legend
    html += '<h3 class="viz-heading">Operator Reference</h3>';
    html += this.renderLegend();

    html += '</div>';
    return html;
  },

  _escHtml(s) { return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;'); },
  _escSvg(s) { return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;'); },
};
