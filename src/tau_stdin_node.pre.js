if (typeof process !== 'undefined' && process.versions && process.versions.node) {
  if (!Module['stdin']) {
    var tau_stdin_fs = require('fs');
    var tau_stdin_buf = Buffer.alloc(4096);
    var tau_stdin_pos = 0;
    var tau_stdin_len = 0;
    var tau_stdin_eof = false;
    Module['stdin'] = function () {
      if (tau_stdin_eof) {
        return null;
      }
      while (tau_stdin_pos >= tau_stdin_len) {
        try {
          tau_stdin_len = tau_stdin_fs.readSync(0, tau_stdin_buf, 0, tau_stdin_buf.length, null);
          tau_stdin_pos = 0;
        } catch (e) {
          if (e.code === 'EOF') {
            tau_stdin_eof = true;
            return null;
          }
          if (e.code === 'EAGAIN') {
            continue;
          }
          throw e;
        }
        if (tau_stdin_len === 0) {
          tau_stdin_eof = true;
          return null;
        }
      }
      return tau_stdin_buf[tau_stdin_pos++];
    };
  }

  // Full-root host filesystem mirror, matching the parser's node host
  // (external/parser/js/tau-wasm-terminal/node.js): a host "/x" is reachable
  // at "/host/x", and relative paths resolve against the caller's cwd. A wasm
  // module has no other way to open a host file.
  //
  // This leaves the batch stdin contract above untouched: readSync still
  // returns null only on real EOF, a NUL byte is still a data byte, and
  // EAGAIN still continues.
  var tau_host_fs = require('fs');
  var tau_host_arg = function (arg) {
    return (arg && arg.charAt(0) === '/' && tau_host_fs.existsSync(arg))
      ? '/host' + arg : arg;
  };
  Module['arguments'] = (Module['arguments'] || process.argv.slice(2))
    .map(tau_host_arg);
  for (var tau_argv = 2; tau_argv < process.argv.length; ++tau_argv)
    process.argv[tau_argv] = tau_host_arg(process.argv[tau_argv]);
  Module['preRun'] = (Module['preRun'] || []).slice();
  Module['preRun'].push(function () {
    FS.mkdir('/host');
    FS.mount(NODEFS, { root: '/' }, '/host');
    FS.chdir('/host' + process.cwd());

    // Transparent full-root fallback. A spec string can carry an absolute host
    // path (e.g. `in file("/home/.../x.in")`) that the VFS does not hold. On a
    // failed lookup whose directory is absent from the VFS, retry the path
    // under /host, which NODEFS mirrors. A path whose directory IS in the VFS
    // (e.g. /tmp) keeps resolving in the VFS, so the mount cannot shadow it.
    var tau_lookup_path = FS.lookupPath;
    // '/host'+path when the path is absolute, outside /host, its directory is
    // absent from the VFS, and that directory exists on the host; else null.
    var tau_host_fallback = function (path) {
      if (typeof path !== 'string' || path.charAt(0) !== '/'
          || path.indexOf('/host/') === 0)
        return null;
      var tau_dir = path.substring(0, path.lastIndexOf('/')) || '/';
      try {
        tau_lookup_path.call(FS, tau_dir, { follow: true });
      } catch (e) {
        // tau_dir is the original host path; a mapped /host+dir does not
        // exist on the host filesystem.
        if (tau_host_fs.existsSync(tau_dir))
          return '/host' + path;
      }
      return null;
    };
    FS.lookupPath = function (path, opts) {
      try {
        // FS.open passes noent_okay, so a missing path yields a result with no
        // node instead of throwing; handle both the result and the throw.
        var result = tau_lookup_path.call(FS, path, opts);
        if (result && result.node) return result;
        var mapped = tau_host_fallback(path);
        return mapped ? tau_lookup_path.call(FS, mapped, opts) : result;
      } catch (e) {
        var mapped = tau_host_fallback(path);
        if (mapped) return tau_lookup_path.call(FS, mapped, opts);
        throw e;
      }
    };
  });

  // Emscripten's node shell starts ENV empty and never reads process.env, so
  // getenv() in the module sees none of the TAU_LTL_* variables and falls
  // back to compiled defaults. Seed ENV from the node environment before main
  // runs; preRun runs after the module scope has assigned ENV, before
  // callMain, so getEnvStrings() picks the values up.
  Module['preRun'].push(function () {
    if (typeof ENV === 'undefined' || !process.env) return;
    for (var tau_env in process.env) {
      if (typeof process.env[tau_env] === 'string' && !(tau_env in ENV))
        ENV[tau_env] = process.env[tau_env];
    }
  });
}
