/**
 * Tau language syntax highlighting and autocomplete — grammar-driven.
 *
 * At startup, fetches token categories from /api/grammar/tokens
 * (derived from parser/tau.tgf) so the highlighter always matches
 * the actual grammar.  Falls back to built-in defaults if the
 * fetch fails.
 */
const TauSyntax = {
  /* ---- token sets (populated from grammar) ---- */
  keywords: [
    'always', 'sometimes', 'all', 'ex', 'fall', 'fex',
    'in', 'out', 'console', 'file',
    'fallback', 'first', 'last',
  ],
  temporalOps: ['G', 'F', 'U', 'R', 'W', 'S', 'T'],
  ctlStarOps: ['A', 'E'],
  commands: [
    'help', 'quit', 'version', 'clear', 'valid', 'sat', 'unsat',
    'solve', 'lgrs', 'run', 'ltl', 'normalize',
    'subst', 'substitute', 'inst', 'instantiate',
    'dnf', 'cnf', 'anf', 'nnf', 'pnf', 'mnf', 'onf', 'qelim',
    'whatis', 'reset', 'fragment', 'func',
    'get', 'set', 'enable', 'disable', 'toggle',
    'defs', 'definitions', 'hist', 'history',
  ],
  constants: ['T', 'F'],
  types: ['bv', 'sbf', 'qlt', 'qint', 'nlang', 'tau'],
  fragments: ['ltl', 'ctl_star'],

  /* Full grammar reference (fetched) */
  grammarData: null,

  /* Autocomplete entries (fetched from grammar) */
  grammarCompletions: null,

  /**
   * Fetch grammar-derived token sets and autocomplete entries.
   * Called once at IDE startup.
   */
  async loadFromGrammar() {
    try {
      const [tokResp, acResp, gramResp] = await Promise.all([
        fetch('/api/grammar/tokens'),
        fetch('/api/grammar/autocomplete'),
        fetch('/api/grammar'),
      ]);
      if (tokResp.ok) {
        const tok = await tokResp.json();
        if (tok.keywords)      this.keywords      = tok.keywords;
        if (tok.temporal_ops)  this.temporalOps    = tok.temporal_ops;
        if (tok.ctl_star_ops)  this.ctlStarOps     = tok.ctl_star_ops;
        if (tok.commands)      this.commands       = tok.commands;
        if (tok.types)         this.types          = tok.types;
        if (tok.fragments)     this.fragments      = tok.fragments;
      }
      if (acResp.ok) {
        this.grammarCompletions = await acResp.json();
      }
      if (gramResp.ok) {
        this.grammarData = await gramResp.json();
      }
    } catch (e) {
      console.warn('Grammar fetch failed, using built-in defaults:', e);
    }
  },

  /* ---- syntax highlighting ---- */

  highlight(source) {
    if (!source) return '';
    let html = '';
    let i = 0;
    const src = source;
    const len = src.length;

    while (i < len) {
      // Comments (from grammar: comment => '#' ...)
      if (src[i] === '#') {
        let end = src.indexOf('\n', i);
        if (end === -1) end = len;
        html += `<span class="hl-comment">${escHtml(src.slice(i, end))}</span>`;
        i = end;
        continue;
      }

      // Strings (from grammar: q_file_name => '"' ...)
      if (src[i] === '"') {
        let end = i + 1;
        while (end < len && src[end] !== '"') {
          if (src[end] === '\\') end++;
          end++;
        }
        if (end < len) end++;
        html += `<span class="hl-string">${escHtml(src.slice(i, end))}</span>`;
        i = end;
        continue;
      }

      // BA constants (from grammar: ba_constant => '{' _ source _ '}')
      if (src[i] === '{') {
        let depth = 1, end = i + 1;
        while (end < len && depth > 0) {
          if (src[end] === '{') depth++;
          else if (src[end] === '}') depth--;
          end++;
        }
        html += `<span class="hl-constant">${escHtml(src.slice(i, end))}</span>`;
        i = end;
        continue;
      }

      // History refs (from grammar: history => '%' ...)
      if (src[i] === '%') {
        let end = i + 1;
        if (end < len && src[end] === '-') end++;
        while (end < len && src[end] >= '0' && src[end] <= '9') end++;
        html += `<span class="hl-history">${escHtml(src.slice(i, end))}</span>`;
        i = end;
        continue;
      }

      // Captures (from grammar: capture => '$' chars)
      if (src[i] === '$') {
        let end = i + 1;
        while (end < len && /[a-zA-Z0-9_]/.test(src[end])) end++;
        html += `<span class="hl-capture">${escHtml(src.slice(i, end))}</span>`;
        i = end;
        continue;
      }

      // Uninterpreted constants (from grammar: uconst => '<' ... '>')
      if (src[i] === '<' && i + 1 < len && /[a-zA-Z_:]/.test(src[i + 1])) {
        let end = src.indexOf('>', i);
        if (end !== -1) {
          end++;
          html += `<span class="hl-uconst">${escHtml(src.slice(i, end))}</span>`;
          i = end;
          continue;
        }
      }

      // Constraint brackets (from grammar: constraint => '[' ctnvar ... ']')
      if (src[i] === '[') {
        let end = src.indexOf(']', i);
        if (end !== -1) {
          end++;
          const inner = src.slice(i, end);
          // Check if it looks like a constant-time constraint [var op num]
          if (/^\[\s*\w+\s*(!=|>=|<=|>|<|=)\s*\d+\s*\]$/.test(inner) ||
              /^\[\s*\d+\s*(!=|>=|<=|>|<|=)\s*\w+\s*\]$/.test(inner)) {
            html += `<span class="hl-constraint">${escHtml(inner)}</span>`;
            i = end;
            continue;
          }
        }
      }

      // Numbers
      if (/[0-9]/.test(src[i])) {
        let end = i;
        while (end < len && /[0-9]/.test(src[end])) end++;
        if (i > 0 && /[a-zA-Z_]/.test(src[i - 1])) {
          html += escHtml(src[i]);
          i++;
          continue;
        }
        html += `<span class="hl-number">${escHtml(src.slice(i, end))}</span>`;
        i = end;
        continue;
      }

      // Identifiers / keywords / commands / types
      if (/[a-zA-Z_]/.test(src[i])) {
        let end = i + 1;
        while (end < len && /[a-zA-Z0-9_]/.test(src[end])) end++;
        const word = src.slice(i, end);

        if (this.commands.includes(word)) {
          html += `<span class="hl-command">${escHtml(word)}</span>`;
        } else if (this.keywords.includes(word)) {
          html += `<span class="hl-keyword">${escHtml(word)}</span>`;
        } else if (word.length === 1 && this.temporalOps.includes(word)) {
          html += `<span class="hl-temporal">${escHtml(word)}</span>`;
        } else if (word.length === 1 && this.ctlStarOps.includes(word)) {
          html += `<span class="hl-temporal">${escHtml(word)}</span>`;
        } else if (word.length === 1 && this.constants.includes(word)) {
          html += `<span class="hl-bool">${escHtml(word)}</span>`;
        } else if (this.types.includes(word)) {
          html += `<span class="hl-type">${escHtml(word)}</span>`;
        } else if (this.fragments.includes(word)) {
          html += `<span class="hl-fragment">${escHtml(word)}</span>`;
        } else {
          html += `<span class="hl-ident">${escHtml(word)}</span>`;
        }
        i = end;
        continue;
      }

      // Multi-char operators (derived from grammar wff/bf productions)
      if (i + 3 <= len && src.slice(i, i + 3) === '<->') {
        html += `<span class="hl-operator">&lt;-&gt;</span>`;
        i += 3;
        continue;
      }
      if (i + 3 <= len && src.slice(i, i + 3) === '!<=') {
        html += `<span class="hl-operator">!&lt;=</span>`;
        i += 3;
        continue;
      }
      if (i + 3 <= len && src.slice(i, i + 3) === '!>=') {
        html += `<span class="hl-operator">!&gt;=</span>`;
        i += 3;
        continue;
      }
      if (i + 2 <= len) {
        const two = src.slice(i, i + 2);
        if ([':=', '->', '<-', '<=', '>=', '!=', '||', '&&', '^^',
             '!|', '!^', '!&', '<>', '[]', '>>', '<<', '!<', '!>'].includes(two)) {
          html += `<span class="hl-operator">${escHtml(two)}</span>`;
          i += 2;
          continue;
        }
      }
      if ('=!<>|^&+-*/%\''.includes(src[i])) {
        html += `<span class="hl-operator">${escHtml(src[i])}</span>`;
        i++;
        continue;
      }

      // Brackets / parens
      if ('()[]'.includes(src[i])) {
        html += `<span class="hl-bracket">${escHtml(src[i])}</span>`;
        i++;
        continue;
      }

      // Period (statement terminator in grammar)
      if (src[i] === '.') {
        html += `<span class="hl-period">.</span>`;
        i++;
        continue;
      }

      // Everything else
      html += escHtml(src[i]);
      i++;
    }

    return html;
  },

  /**
   * Get autocomplete suggestions at cursor position.
   * Uses grammar-derived completions when available.
   */
  getCompletions(source, cursorPos) {
    let start = cursorPos - 1;
    while (start >= 0 && /[a-zA-Z_0-9]/.test(source[start])) start--;
    start++;
    const prefix = source.slice(start, cursorPos).toLowerCase();
    if (!prefix) return [];

    // Use grammar-fetched completions if available
    if (this.grammarCompletions) {
      return this.grammarCompletions
        .filter(w => w.label.toLowerCase().startsWith(prefix))
        .slice(0, 20);
    }

    // Fallback to built-in lists
    const allWords = [
      ...this.keywords.map(w => ({ label: w, type: 'keyword', doc: '' })),
      ...this.temporalOps.map(w => ({ label: w, type: 'temporal', doc: 'LTL temporal operator' })),
      ...this.ctlStarOps.map(w => ({ label: w, type: 'ctl_star', doc: 'CTL* path quantifier' })),
      ...this.commands.map(w => ({ label: w, type: 'command', doc: '' })),
      ...this.types.map(w => ({ label: w, type: 'type', doc: '' })),
      ...this.fragments.map(w => ({ label: w, type: 'fragment', doc: '' })),
    ];

    return allWords
      .filter(w => w.label.toLowerCase().startsWith(prefix))
      .slice(0, 20);
  },

  /**
   * Look up grammar documentation for a token.
   * Returns { syntax, doc, category } or null.
   */
  lookupToken(word) {
    if (!this.grammarData) return null;

    // Search wff rules
    for (const rule of (this.grammarData.wff_rules || [])) {
      if (rule.syntax && rule.syntax.includes(word)) {
        return { ...rule, category: 'wff' };
      }
    }
    // Search bf rules
    for (const rule of (this.grammarData.bf_rules || [])) {
      if (rule.syntax && rule.syntax.includes(word)) {
        return { ...rule, category: 'bf' };
      }
    }
    // Search cli rules
    for (const rule of (this.grammarData.cli_rules || [])) {
      if (rule.aliases && rule.aliases.includes(word)) {
        return { ...rule, category: 'command' };
      }
    }
    return null;
  },

  /**
   * Get the raw grammar text.
   */
  getRawGrammar() {
    return this.grammarData ? this.grammarData.raw : null;
  },

  /**
   * Get all grammar rules categorized for the help panel.
   */
  getGrammarReference() {
    if (!this.grammarData) return null;
    return {
      wff: this.grammarData.wff_rules || [],
      bf: this.grammarData.bf_rules || [],
      commands: this.grammarData.cli_rules || [],
      constraints: this.grammarData.constraint_rules || [],
      keywords: this.grammarData.keywords || [],
      temporal_ops: this.grammarData.temporal_ops || [],
      ctl_star_ops: this.grammarData.ctl_star_ops || [],
      types: this.grammarData.types || [],
      wff_ops: this.grammarData.wff_ops || [],
      bf_ops: this.grammarData.bf_ops || [],
      relational_ops: this.grammarData.relational_ops || [],
      fragments: this.grammarData.fragments || [],
    };
  },
};

function escHtml(s) {
  return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

// Highlighting + grammar-reference styles
const highlightCSS = `
.hl-comment    { color: #6c7086; font-style: italic; }
.hl-keyword    { color: #cba6f7; font-weight: 600; }
.hl-temporal   { color: #f38ba8; font-weight: 700; }
.hl-command    { color: #89b4fa; }
.hl-bool       { color: #fab387; font-weight: 700; }
.hl-type       { color: #94e2d5; }
.hl-fragment   { color: #f9e2af; }
.hl-constant   { color: #a6e3a1; }
.hl-string     { color: #a6e3a1; }
.hl-number     { color: #fab387; }
.hl-operator   { color: #89dceb; }
.hl-bracket    { color: #f9e2af; }
.hl-period     { color: #f5c2e7; font-weight: 700; }
.hl-ident      { color: #cdd6f4; }
.hl-capture    { color: #f5c2e7; }
.hl-history    { color: #74c7ec; font-weight: 600; }
.hl-uconst     { color: #eba0ac; }
.hl-constraint { color: #94e2d5; font-style: italic; }

/* grammar reference table */
.grammar-table        { width: 100%; border-collapse: collapse; font-size: 12px; }
.grammar-table th     { text-align: left; padding: 4px 8px; border-bottom: 1px solid #313244;
                         color: #a6adc8; font-size: 11px; text-transform: uppercase; }
.grammar-table td     { padding: 4px 8px; border-bottom: 1px solid #1e1e2e; }
.grammar-table tr:hover td { background: #1e1e2e; }
.grammar-syntax       { font-family: 'JetBrains Mono', monospace; color: #89b4fa; }
.grammar-label        { color: #6c7086; font-size: 11px; }
.grammar-doc          { color: #cdd6f4; }
.grammar-category     { display: inline-block; padding: 1px 6px; border-radius: 3px;
                         font-size: 10px; font-weight: 600; text-transform: uppercase; }
.grammar-category.wff { background: #45475a; color: #cba6f7; }
.grammar-category.bf  { background: #45475a; color: #89b4fa; }
.grammar-category.cmd { background: #45475a; color: #a6e3a1; }
.grammar-category.ctn { background: #45475a; color: #94e2d5; }

/* tooltip */
.token-tooltip {
  position: absolute;
  background: #1e1e2e;
  border: 1px solid #45475a;
  border-radius: 6px;
  padding: 8px 12px;
  font-size: 12px;
  max-width: 360px;
  z-index: 9999;
  box-shadow: 0 4px 16px rgba(0,0,0,0.4);
  pointer-events: none;
}
.token-tooltip .tt-syntax  { color: #89b4fa; font-family: monospace; margin-bottom: 4px; }
.token-tooltip .tt-doc     { color: #cdd6f4; }
.token-tooltip .tt-cat     { color: #6c7086; font-size: 11px; }
`;

const styleEl = document.createElement('style');
styleEl.textContent = highlightCSS;
document.head.appendChild(styleEl);
