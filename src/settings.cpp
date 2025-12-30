#include "settings.hpp"

#include "properties_file.hpp"
#include "text.hpp"

#include <memory>

namespace robikzinputtest {

/*
  Property import/export
*/

class PropImportExport {
public:
	virtual ~PropImportExport() = default;
	virtual std::string name() const = 0;
	virtual bool is_property(const std::string &name) const = 0;
	virtual void import_value(const Variant &value) = 0;
	virtual Variant export_value() const = 0;
};

template<typename T>
class PropRef : public PropImportExport {
public:
	PropRef(const std::string &name, T &ref)
		: m_name(name), m_ref(ref) {
	}

	std::string name() const override {
		return m_name;
	}

	bool is_property(const std::string &name) const override {
		return text::tolower(m_name) == text::tolower(name);
	}

	void import_value(const Variant &value) override {
		m_ref = static_cast<T>(value);
	}

	Variant export_value() const override {
		return Variant(m_ref);
	}

private:
	std::string m_name;
	T &m_ref;
};

static std::unique_ptr<PropRef<bool>> boolprop(const std::string &name, bool &ref) {
	return std::make_unique<PropRef<bool>>(name, ref);
}

static std::unique_ptr<PropRef<int>> intprop(const std::string &name, int &ref) {
	return std::make_unique<PropRef<int>>(name, ref);
}

static std::unique_ptr<PropRef<float>> floatprop(const std::string &name, float &ref) {
	return std::make_unique<PropRef<float>>(name, ref);
}

static std::unique_ptr<PropRef<Color>> colorprop(const std::string &name, Color &ref) {
	return std::make_unique<PropRef<Color>>(name, ref);
}

std::vector<std::unique_ptr<PropImportExport>> create_settings_prop_map(Settings &settings) {
	std::vector<std::unique_ptr<PropImportExport>> props;
	props.push_back(boolprop("show_fps", settings.show_fps));
	props.push_back(boolprop("show_ui_frame_counter", settings.show_ui_frame_counter));
	props.push_back(boolprop("show_help", settings.show_help));
	props.push_back(boolprop("show_help_at_start", settings.show_help_at_start));
	props.push_back(boolprop("show_settings_at_start", settings.show_settings_at_start));
	props.push_back(boolprop("show_program_log", settings.show_program_log));
	props.push_back(boolprop("show_joystick_info", settings.show_joystick_info));
	props.push_back(floatprop("target_fps", settings.target_fps));
	props.push_back(intprop("gizmo_width", settings.gizmo_width));
	props.push_back(intprop("gizmo_height", settings.gizmo_height));
	props.push_back(intprop("joystick_deadzone", settings.joystick_deadzone));
	props.push_back(colorprop("background_color", settings.background_color));
	props.push_back(boolprop("background_animate", settings.background_animate));
	props.push_back(colorprop("background_flash_color", settings.background_flash_color));
	props.push_back(boolprop("background_flash_on_gizmo_action", settings.background_flash_on_gizmo_action));
	return props;
}

/*
  Settings
*/

void Settings::import_properties(const std::vector<Property> &properties) {
	auto props_map = create_settings_prop_map(*this);
	for (const auto &prop : properties) {
		for (const auto &pmap : props_map) {
			if (pmap->is_property(prop.name)) {
				pmap->import_value(prop.value);
				break;
			}
		}
	}
}

std::vector<Property> Settings::export_properties() const {
	auto props_map = create_settings_prop_map(const_cast<Settings&>(*this));
	std::vector<Property> properties;
	for (const auto &pmap : props_map) {
		Property prop;
		prop.name = pmap->name();
		prop.value = pmap->export_value();
		properties.push_back(prop);
	}
	return properties;
}

} // namespace robikzinputtest
