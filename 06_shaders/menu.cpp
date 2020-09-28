//
// Created by fede on 04/09/20.
//

//TODO remove smooth/flat shading?
typedef enum {
    WIRE_FRAME,
    FACE_FILL,
    FLAT_SHADING,
    SMOOTH_SHADING,
    CULLING_ON,
    CULLING_OFF,
    CHANGE_TO_WCS,
    CHANGE_TO_OCS
} MenuOption;

// gestione delle voci principali del menu
void main_menu_func(int option) {
    switch (option) {
        case MenuOption::FLAT_SHADING:
            glShadeModel(GL_FLAT);
            break;
        case MenuOption::SMOOTH_SHADING:
            glShadeModel(GL_SMOOTH);
            break;
        case MenuOption::WIRE_FRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case MenuOption::FACE_FILL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case MenuOption::CULLING_ON:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);	// remove faces facing the background
            break;
        case MenuOption::CULLING_OFF:
            glDisable(GL_CULL_FACE);
            break;
        case MenuOption::CHANGE_TO_OCS:
            TransformMode = OCS;
            break;
        case MenuOption::CHANGE_TO_WCS:
            TransformMode = WCS;
            break;
        default:
            break;
    }
}

// gestione delle voci principali del sub menu per i matriali
void material_menu_function(int option) {
    switch (option) {
        case MaterialType::RED_PLASTIC:
            objects[selected_object].material = MaterialType::RED_PLASTIC;
            break;
        case MaterialType::EMERALD:
            objects[selected_object].material = MaterialType::EMERALD;
            break;
        case MaterialType::BRASS:
            objects[selected_object].material = MaterialType::BRASS;
            break;
        case MaterialType::SLATE:
            objects[selected_object].material = MaterialType::SLATE;
            break;
        default:
            break;
    }
}

// costruisce i menu openGL
void buildOpenGLMenu() {
    int materialSubMenu = glutCreateMenu(material_menu_function);

    glutAddMenuEntry(materials[MaterialType::RED_PLASTIC].name.c_str(), MaterialType::RED_PLASTIC);
    glutAddMenuEntry(materials[MaterialType::EMERALD].name.c_str(), MaterialType::EMERALD);
    glutAddMenuEntry(materials[MaterialType::BRASS].name.c_str(), MaterialType::BRASS);
    glutAddMenuEntry(materials[MaterialType::SLATE].name.c_str(), MaterialType::SLATE);


    glutCreateMenu(main_menu_func); // richiama main_menu_func() alla selezione di una voce menu
    glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga
    glutAddMenuEntry("", -1);
    glutAddMenuEntry("Wireframe", MenuOption::WIRE_FRAME);
    glutAddMenuEntry("Face fill", MenuOption::FACE_FILL);
    glutAddMenuEntry("Smooth Shading", MenuOption::SMOOTH_SHADING);
    glutAddMenuEntry("Flat Shading", MenuOption::FLAT_SHADING);
    glutAddMenuEntry("Culling: ON", MenuOption::CULLING_ON);
    glutAddMenuEntry("Culling: OFF", MenuOption::CULLING_OFF);
    glutAddSubMenu("Material", materialSubMenu);
    glutAddMenuEntry("World coordinate system", MenuOption::CHANGE_TO_WCS);
    glutAddMenuEntry("Object coordinate system", MenuOption::CHANGE_TO_OCS);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}