use std::env;
use std::error::Error;
use std::path::{Path, PathBuf};
use std::process::Command;

type AnyError = Box<dyn Error>;
type Result<T> = std::result::Result<T, AnyError>;

fn main() -> Result<()> {
    let root = project_root()?;
    let args: Vec<String> = env::args().skip(1).collect();

    if args.is_empty() {
        print_help();
        return Ok(());
    }

    match args[0].as_str() {
        "help" | "-h" | "--help" => {
            print_help();
            Ok(())
        }
        "audit" => cmd_audit(&root),
        "sandbox-run" => cmd_sandbox_run(&root, &args[1..]),
        "sdk-build" => cmd_sdk_build(&root, &args[1..]),
        "sdk-install-core" => cmd_sdk_install_core(&root, &args[1..]),
        "sdk-launch" => cmd_sdk_launch(&root, &args[1..]),
        "sdk-sync-patches" => cmd_sdk_sync_patches(&root, &args[1..]),
        other => Err(format!("unknown xtask command: {other}").into()),
    }
}

fn print_help() {
    println!("Retrospective xtask");
    println!();
    println!("Usage:");
    println!("  cargo xtask <command> [options] [-- passthrough-args]");
    println!();
    println!("Commands:");
    println!("  help                 Show this help");
    println!("  audit                Run Rust tests");
    println!("  sandbox-run          Build and run standalone C++ playground");
    println!("  sdk-build            Build source-sdk-2013 (Linux and Windows paths)");
    println!("  sdk-install-core     Build and copy Rust core library into SDK mod bin");
    println!("  sdk-launch           Launch SDK mod (Linux wrapper today)");
    println!("  sdk-sync-patches     Apply the Retrospective SDK patchset to an SDK checkout");
    println!();
    println!("Examples:");
    println!("  cargo xtask audit");
    println!("  cargo xtask sandbox-run");
    println!("  cargo xtask sdk-sync-patches");
    println!("  cargo xtask sdk-build --mode release");
    println!("  cargo xtask sdk-install-core --target x86_64-unknown-linux-gnu");
}

fn project_root() -> Result<PathBuf> {
    let mut dir = PathBuf::from(env!("CARGO_MANIFEST_DIR"));

    loop {
        if dir.join("Cargo.toml").is_file()
            && dir.join("rust").is_dir()
            && dir.join("native").is_dir()
        {
            return Ok(dir);
        }
        if !dir.pop() {
            break;
        }
    }

    Err("failed to resolve workspace root from xtask manifest path".into())
}

fn cmd_audit(root: &Path) -> Result<()> {
    run(root, "cargo", &["test", "-p", "retrospective_core"])
}

fn cmd_sandbox_run(root: &Path, args: &[String]) -> Result<()> {
    let (_opts, passthrough) = split_passthrough(args);

    let mut configure_args = vec![
        "-S".to_string(),
        "rust/sandbox/cpp_playground".to_string(),
        "-B".to_string(),
        "build/rust_cpp_playground".to_string(),
    ];
    if !cfg!(windows) {
        configure_args.push("-DCMAKE_BUILD_TYPE=Release".to_string());
    }
    run_owned(root, "cmake", &configure_args)?;

    let mut build_args = vec![
        "--build".to_string(),
        "build/rust_cpp_playground".to_string(),
        "--parallel".to_string(),
    ];
    if cfg!(windows) {
        build_args.push("--config".to_string());
        build_args.push("Release".to_string());
    }
    run_owned(root, "cmake", &build_args)?;

    let binary_path = if cfg!(windows) {
        root.join("build")
            .join("rust_cpp_playground")
            .join("Release")
            .join(format!("cpp_playground{}", env::consts::EXE_SUFFIX))
    } else {
        root.join("build")
            .join("rust_cpp_playground")
            .join("cpp_playground")
    };
    if !binary_path.exists() {
        return Err(format!("missing playground binary: {}", binary_path.display()).into());
    }

    run_path(root, &binary_path, &passthrough)?;
    Ok(())
}

fn cmd_sdk_build(root: &Path, args: &[String]) -> Result<()> {
    let mut sdk_dir = root.join("external").join("source-sdk-2013");
    let mut mode = "release".to_string();

    let mut i = 0usize;
    while i < args.len() {
        match args[i].as_str() {
            "--sdk-dir" => {
                i += 1;
                if i >= args.len() {
                    return Err("--sdk-dir expects a path".into());
                }
                sdk_dir = PathBuf::from(&args[i]);
                i += 1;
            }
            "--mode" => {
                i += 1;
                if i >= args.len() {
                    return Err("--mode expects release or debug".into());
                }
                mode = args[i].clone();
                i += 1;
            }
            other => return Err(format!("unknown option for sdk-build: {other}").into()),
        }
    }

    if mode != "release" && mode != "debug" {
        return Err("--mode must be release or debug".into());
    }

    let src_dir = sdk_dir.join("src");
    if !src_dir.is_dir() {
        return Err(format!("source-sdk-2013 src dir not found at {}", src_dir.display()).into());
    }

    if cfg!(windows) {
        run(&src_dir, "cmd", &["/C", "createallprojects.bat"])?;
        let configuration = if mode == "release" {
            "Release"
        } else {
            "Debug"
        };
        run(
            &src_dir,
            "msbuild",
            &[
                "everything.sln",
                "/m",
                &format!("/p:Configuration={configuration}"),
            ],
        )?;
    } else {
        run(&src_dir, "./buildallprojects", &[mode.as_str()])?;
    }

    Ok(())
}

fn cmd_sdk_install_core(root: &Path, args: &[String]) -> Result<()> {
    let mut sdk_dir = root.join("external").join("source-sdk-2013");
    let mut mod_name = "mod_hl2mp".to_string();
    let mut target: Option<String> = None;

    let mut i = 0usize;
    while i < args.len() {
        match args[i].as_str() {
            "--sdk-dir" => {
                i += 1;
                if i >= args.len() {
                    return Err("--sdk-dir expects a path".into());
                }
                sdk_dir = PathBuf::from(&args[i]);
                i += 1;
            }
            "--mod-name" => {
                i += 1;
                if i >= args.len() {
                    return Err("--mod-name expects a value".into());
                }
                mod_name = args[i].clone();
                i += 1;
            }
            "--target" => {
                i += 1;
                if i >= args.len() {
                    return Err("--target expects a triple".into());
                }
                target = Some(args[i].clone());
                i += 1;
            }
            other => return Err(format!("unknown option for sdk-install-core: {other}").into()),
        }
    }

    if !sdk_dir.join("src").is_dir() {
        return Err(format!("source-sdk-2013 not found at {}", sdk_dir.display()).into());
    }

    let target = match target {
        Some(t) => t,
        None => detect_host_triple(root)?,
    };

    run(
        root,
        "cargo",
        &[
            "build",
            "-p",
            "retrospective_core",
            "--release",
            "--target",
            &target,
        ],
    )?;

    let built_name = lib_name_for_target(&target)?;
    let built_lib_path = root
        .join("target")
        .join(&target)
        .join("release")
        .join(&built_name);
    if !built_lib_path.is_file() {
        return Err(format!("built core library not found: {}", built_lib_path.display()).into());
    }

    let dest_dir = sdk_core_dest_dir(&sdk_dir, &mod_name, &target)?;
    std::fs::create_dir_all(&dest_dir)?;
    let dest_file = dest_dir.join(&built_name);
    std::fs::copy(&built_lib_path, &dest_file)?;

    println!(
        "installed core library:\n  from: {}\n  to:   {}",
        built_lib_path.display(),
        dest_file.display()
    );
    Ok(())
}

fn cmd_sdk_launch(root: &Path, args: &[String]) -> Result<()> {
    let (opts, passthrough) = split_passthrough(args);
    let mut sdk_dir = root.join("external").join("source-sdk-2013");

    let mut i = 0usize;
    while i < opts.len() {
        match opts[i].as_str() {
            "--sdk-dir" => {
                i += 1;
                if i >= opts.len() {
                    return Err("--sdk-dir expects a path".into());
                }
                sdk_dir = PathBuf::from(&opts[i]);
                i += 1;
            }
            other => return Err(format!("unknown option for sdk-launch: {other}").into()),
        }
    }

    if cfg!(windows) {
        return Err(
            "sdk-launch currently has only Linux automation. On Windows, use the Visual Studio startup profile generated by Source SDK."
                .into(),
        );
    }

    let script = root
        .join("scripts")
        .join("phase2_launch_hl2mp_with_core.sh");
    if !script.is_file() {
        return Err(format!("missing launch script: {}", script.display()).into());
    }

    let mut launch_args = Vec::new();
    launch_args.push(sdk_dir.display().to_string());
    launch_args.extend(passthrough);
    run_path(root, &script, &launch_args)?;
    Ok(())
}

fn cmd_sdk_sync_patches(root: &Path, args: &[String]) -> Result<()> {
    let mut sdk_dir = root.join("external").join("source-sdk-2013");

    let mut i = 0usize;
    while i < args.len() {
        match args[i].as_str() {
            "--sdk-dir" => {
                i += 1;
                if i >= args.len() {
                    return Err("--sdk-dir expects a path".into());
                }
                sdk_dir = PathBuf::from(&args[i]);
                i += 1;
            }
            other => return Err(format!("unknown option for sdk-sync-patches: {other}").into()),
        }
    }

    if !sdk_dir.join(".git").exists() {
        return Err(format!("not a git checkout: {}", sdk_dir.display()).into());
    }

    let patch_file = root
        .join("patches")
        .join("source-sdk-2013")
        .join("0001-wire-retrospective-sdk-bridge.patch");
    if !patch_file.is_file() {
        return Err(format!("missing patch file: {}", patch_file.display()).into());
    }

    let clean_output = Command::new("git")
        .args(["status", "--porcelain", "--untracked-files=no"])
        .current_dir(&sdk_dir)
        .output()?;
    if !clean_output.status.success() {
        return Err("failed to run git status in SDK checkout".into());
    }
    if !String::from_utf8(clean_output.stdout)?.trim().is_empty() {
        return Err(
            "sdk-sync-patches requires a clean SDK working tree (tracked files only)".into(),
        );
    }

    let patch_arg = patch_file.to_string_lossy().into_owned();

    let reverse_check = Command::new("git")
        .args(["apply", "--check", "--reverse", &patch_arg])
        .current_dir(&sdk_dir)
        .status()?;
    if reverse_check.success() {
        println!("sdk-sync-patches: patchset already applied; nothing to do.");
        return Ok(());
    }

    let forward_check = Command::new("git")
        .args(["apply", "--check", &patch_arg])
        .current_dir(&sdk_dir)
        .status()?;
    if !forward_check.success() {
        return Err("patchset does not apply cleanly (upstream drift or incompatible base)".into());
    }

    run_owned(
        &sdk_dir,
        "git",
        &["apply".to_string(), "--3way".to_string(), patch_arg.clone()],
    )?;

    println!(
        "sdk-sync-patches: applied patchset.\nnext: inspect with `git -C {} status --short`",
        sdk_dir.display()
    );

    Ok(())
}

fn sdk_core_dest_dir(sdk_dir: &Path, mod_name: &str, target: &str) -> Result<PathBuf> {
    let base = sdk_dir.join("game").join(mod_name).join("bin");
    if target == "x86_64-unknown-linux-gnu" {
        Ok(base.join("linux64").join("retrospective"))
    } else if target == "i686-unknown-linux-gnu" {
        Ok(base.join("linux32").join("retrospective"))
    } else if target.contains("windows") {
        Ok(base.join("retrospective"))
    } else {
        Err(format!("unsupported target for sdk-install-core: {target}").into())
    }
}

fn lib_name_for_target(target: &str) -> Result<String> {
    if target.contains("windows") {
        Ok("retrospective_core.dll".to_string())
    } else if target.contains("apple") || target.contains("darwin") {
        Ok("libretrospective_core.dylib".to_string())
    } else if target.contains("linux") {
        Ok("libretrospective_core.so".to_string())
    } else {
        Err(format!("unsupported target triple for library naming: {target}").into())
    }
}

fn detect_host_triple(root: &Path) -> Result<String> {
    let output = Command::new("rustc")
        .arg("-vV")
        .current_dir(root)
        .output()?;
    if !output.status.success() {
        return Err("failed to run rustc -vV".into());
    }
    let stdout = String::from_utf8(output.stdout)?;
    for line in stdout.lines() {
        if let Some(host) = line.strip_prefix("host: ") {
            return Ok(host.trim().to_string());
        }
    }
    Err("could not parse host triple from rustc -vV output".into())
}

fn split_passthrough(args: &[String]) -> (Vec<String>, Vec<String>) {
    if let Some(index) = args.iter().position(|arg| arg == "--") {
        (args[..index].to_vec(), args[index + 1..].to_vec())
    } else {
        (args.to_vec(), Vec::new())
    }
}

fn run(cwd: &Path, program: &str, args: &[&str]) -> Result<()> {
    let owned: Vec<String> = args.iter().map(|s| (*s).to_string()).collect();
    run_owned(cwd, program, &owned)
}

fn run_owned(cwd: &Path, program: &str, args: &[String]) -> Result<()> {
    let rendered_args = args
        .iter()
        .map(|arg| quote_for_log(arg))
        .collect::<Vec<_>>()
        .join(" ");
    println!("+ (cd {}) {} {}", cwd.display(), program, rendered_args);

    let status = Command::new(program).args(args).current_dir(cwd).status()?;
    if !status.success() {
        return Err(format!("{program} exited with status {status}").into());
    }
    Ok(())
}

fn run_path(cwd: &Path, program: &Path, args: &[String]) -> Result<()> {
    let rendered_args = args
        .iter()
        .map(|arg| quote_for_log(arg))
        .collect::<Vec<_>>()
        .join(" ");
    println!(
        "+ (cd {}) {} {}",
        cwd.display(),
        program.display(),
        rendered_args
    );

    let status = Command::new(program).args(args).current_dir(cwd).status()?;
    if !status.success() {
        return Err(format!("{} exited with status {}", program.display(), status).into());
    }
    Ok(())
}

fn quote_for_log(input: &str) -> String {
    if input.chars().any(char::is_whitespace) {
        format!("\"{input}\"")
    } else {
        input.to_string()
    }
}
