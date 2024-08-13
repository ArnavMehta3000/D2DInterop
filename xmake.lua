add_requires("directxtk",
{
	debug =	is_mode("debug"),
	configs =
	{
		runtimes = is_mode("debug") and "MDd" or "MD"
	}
})
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")
set_languages("c17", "cxx23")
add_rules("plugin.vsxmake.autoupdate")

target("App")
	set_kind("binary")
	add_files("**.cpp")
	add_headerfiles("**.h", { install = false })
	add_packages("directxtk")
	add_defines("UNICODE", "NOMINMAX", "WIN32_LEAN_AND_MEAN")
	add_rules("win.sdk.application")
	add_cxxflags("cl::/ZI")
	add_ldflags("-L/INCREMENTAL")

	if is_mode("debug") then
		add_defines("_DEBUG")
		set_runtimes("MDd")
	elseif is_mode("release") then
		add_defines("NDEBUG")
		set_runtimes("MD")
	end
target_end()