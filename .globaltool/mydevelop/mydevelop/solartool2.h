#ifndef SOLARTOOL2_H
#define SOLARTOOL2_H

#include <fstream>
#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/range/combine.hpp>
#include <WinUser.h>

//#include <learnopengl/original/filesystem.h>


//std::string rootCommonDataPath{ "../.common/data/" };
//std::string rootLocalDataPath{ "data/" };

//A class to read data from a csv file.
namespace CSVReader {

	int numRecordsReadIn{ 0 };

	struct MotionDataFormatCSV {
		int repeat{};
		float systimer{};
		int planetId{};
		std::string planetName{};
		float scaledPositionX{};
		float scaledPositionY{};
		float scaledPositionZ{};
		float positionX{};
		float positionY{};
		float positionZ{};
		float velocityX{};
		float velocityY{};
		float velocityZ{};
		float accelerationX{};
		float accelerationY{};
		float accelerationZ{};
	};
	// Function to fetch data from a CSV File
	std::vector<std::vector<std::string> > readFileAll(std::string filename, bool headerpass = true, std::string delm = ",") {
		bool headerPass{ headerpass };
		std::string delimeter{ delm };
		std::ifstream infile(filename);
		std::vector<std::vector<std::string> > datalist;
		if (!infile) {
			std::cerr << "Cannot open the file in func<readFileAl>l: " << filename << std::endl;
			infile.close();
			throw;
		}

		std::string line = "";
		// Iterate through each line and split the content using delimeter
		int count{ 0 };
		numRecordsReadIn = count;
		while (getline(infile, line))
		{
			if (headerPass) { // read trrough the header
				headerPass = false;
				continue;
			}

			std::vector<std::string> vec;
			boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
			datalist.push_back(vec);
			count += 1;

		}
		// Close the File
		infile.close();
		return datalist;
	}

	bool iterateMotionFileByChunk(std::string filename, int chunk, std::function<void(std::vector<MotionDataFormatCSV>&)> callback, bool headerpass = true, std::string delm = ",") {
		bool headerPass{ headerpass };
		std::string delimeter{ delm };
		// open the File 
		std::ifstream infile(filename);
		std::vector<MotionDataFormatCSV> datalist{};

		if (!infile) {
			std::cerr << "Cannot open the file in func<iterateMotionFileByChunk> : "  << filename << std::endl;
			infile.close();
			throw;
		}

		MotionDataFormatCSV element{};
		std::string line{};
		std::vector<std::string> vecline{};
		// Iterate through each line and split the content using delimeter
		int countLine{ 0 };
		numRecordsReadIn = countLine;
		int repetition{ 0 };
		while (std::getline(infile, line)) {
			// Call teh given call back
			if (headerPass) { // read trrough the header
				headerPass = false;
				continue;
			}

			if (countLine > chunk) {
				callback(datalist);
				repetition += 1;
				//std::cout << "Callback Counts: " << repetition << std::endl; // debug
				//printf("\n Separation \n");
				datalist.clear();
				countLine = 1;
				//if (repetition > 3) { // for debug
				//    break;
				//}
				//continue;
			}


			boost::algorithm::split(vecline, line, boost::is_any_of(delimeter));
			element = {
				std::stoi(vecline[0]),
				std::stof(vecline[1]),
				std::stoi(vecline[2]),
				vecline[3],
				std::stof(vecline[4]),
				std::stof(vecline[5]),
				std::stof(vecline[6]),
				std::stof(vecline[7]),
				std::stof(vecline[8]),
				std::stof(vecline[9]),
				std::stof(vecline[10]),
				std::stof(vecline[11]),
				std::stof(vecline[12]),
				std::stof(vecline[13]),
				std::stof(vecline[14]),
				std::stof(vecline[15]),
			};
			vecline.clear();
			datalist.push_back(element);

			countLine += 1;
			numRecordsReadIn += 1;

		}
		// Close the file

		callback(datalist);
		repetition += 1;
		std::cout << "Callback Counts: " << repetition << " Ended" << std::endl; // debug
		infile.close();
		return true;
	}

}

namespace SolarSys {

	const int numberOfLights = 1;

	struct MotionDataFormat {
		int repeat{ 0 };
		float systimer{ 0.0f };
		int planetId{ 0 };
		glm::vec3 scaledPosition{ 0.0f }; // in pixel
		glm::vec3 position{ 0.0f }; // in pixel
		glm::vec3 velocity{ 0.0f }; // in pixel
		glm::vec3 acceleration{ 0.0f }; // in pixel
		glm::vec3 momentum{ 0.0f };
		glm::vec3 orbitalAngularMomentum{ 0.0f };
		glm::vec3 spinAngularMomentum{ 0.0f };
	};

	struct ShaderProgram {
		std::string shaderName{};
		bool toUse{ };
		std::string vert{ };
		std::string frag{ };
		std::string tesc{ };
		std::string tese{ };
		std::string geom{ };
		MyShader::Shader* shaderPtr{};
		GLuint programID{};
		std::vector<std::string> unifLocName{};
		std::vector<GLint> unifLocNo{};
		std::vector<std::string> unifBlockName{};
		std::vector<GLuint> unifBlockBindingIndex{};
		std::vector<GLuint> unifBlockIndex{};
		std::vector<std::string> texUnifLocName{};
		std::vector<GLuint> texUnitNo{};
	};

	struct TextureData {
		std::string texName{};
		std::string texFile{};
		GLuint texUnitNo{};
		std::string unifLocName{};
		GLuint texObject{};
	};

	struct ConstantData {
		int planetId{ 0 };
		std::string planetName{};
		std::string color{};
		float size{ 0.0f };
		std::string shaderName{};
		std::vector<std::string> texName{};
		int satteliteOfId{ 0 };
		glm::vec3 pixelScaler{ 0.0f };
		float mass{ 0.0f }; // Kg
		float radius{ 0.0f }; // Km
		float obliquity{ 0.0f }; // in degree, against its orbit plane
		float inclination{ 0.0f }; // in degree, to the ecliptic - earth orbital plane
		float rotationPeriod{ 0.0f }; // day
		float rotationVelocity{ 0.0f }; //km/s
		float orbitalPeriod{ 0.0f }; // day
		float orbitalSpeed{ 0.0f }; // km/s
		float distanceToParent{ 0.0f }; // km
		float distanceToParentAu{ 0.0f };
		glm::vec3 initPosition{ 0.0f }; // km , i.e. distance from parent
		glm::vec3 initVelocity{ 0.0f }; // m / s
		glm::vec3 initAcceleration{ 0.0f }; // m / s*2
	};


	struct MVPBlock {
		glm::mat4 model{ 1.0f };
		glm::mat4 viewl{ 1.0f };
		glm::mat4 projection{ 1.0f };
	};


	struct MaterialBlock {
		glm::vec4 diffuseColor{ 0.0f };
		glm::vec4 specularColor{ 0.0f };
		float specularShininess{ 0.0f };
		float padding[3]{ 0.0f };
	};

	struct PerLight {
		glm::vec3 lightDirV3InWS{ 0.0f };
		float intraStarsDistance{ 0.0f };
		float attenuCoefficient{ 0.0f };
		float padding[3]{ 0.0f };
		glm::vec4 lightIntensity{ 0.0f };
	};

	struct LightBlock {
		glm::vec4 ambientIntensity{ 0.0f };
		float attenuCoefficient{ 0.0f };
		float maxIntensity{ 0.0f };
		float gamma{ 0.0f };
		float padding{ 0.0f };
		PerLight lights[numberOfLights];
	};

	struct UnifBlocks {
		std::string unifBlockName{};
		GLuint unifBindingIndex{};
		GLuint unifBuffObject{};
		GLsizeiptr unifBlockSize = getUnifBlockSize(unifBindingIndex);
		GLsizeiptr getUnifBlockSize(GLuint i) {
			GLsizeiptr size{ 0 };
			switch (i) {
			case 0: {
				size = sizeof(MVPBlock);
				break;
			}
			case 1: {
				size = sizeof(MaterialBlock);
				break;
			}
			case 3: {
				size = sizeof(LightBlock);
				break;
			}
			}
			return size;
		}
	};

	enum class subDataType {
		model,
		view,
		projection
	};

	struct MVPBlockSubData {
		subDataType subBlockName{};
		GLintptr offset = getOffset(subBlockName);
		GLsizeiptr size = sizeof(glm::mat4);
		GLintptr getOffset(subDataType s) {
			GLintptr  offset{ 0 };
			switch (s) {
			case subDataType::model: {
				offset = sizeof(glm::mat4) * 0;
				break;
			}
			case subDataType::view: {
				offset = sizeof(glm::mat4) * 1;
				break;
			}
			case subDataType::projection: {
				offset = sizeof(glm::mat4) * 2;
				break;
			}
			}
			return offset;
		}
	};


	void genConstantData(std::string filename);
	void genShaderProgram(std::string filename);
	void genTextureObject(std::string filename);

	std::map<int, ConstantData> constant; // key:planetId
	std::map<std::string, ShaderProgram> shader; // key: shadername
	std::map<std::string, TextureData> texture; // key: texName
	std::map<std::string, UnifBlocks> unifBlocks; // key:binding point
	std::map<subDataType, MVPBlockSubData> mvpSubData;
	std::vector<CSVReader::MotionDataFormatCSV> stars{};


	std::string motionFile;
	std::string constantFile;
	std::string shaderFile;
	std::string textureFile;
	int Chunk;

	// for Read_In
	void initialSetup(std::string motionfile, std::string constantfile, std::string shaderfile, std::string texturefile, int chunk = 140) {
		motionFile = motionfile;
		constantFile = constantfile;
		shaderFile = shaderfile;
		textureFile = texturefile;
		Chunk = chunk;

		genConstantData(constantFile);
		genShaderProgram(shaderFile);
		genTextureObject(textureFile);

	}

	// for AlgorismInside
	void initialSetup(std::string constantfile, std::string shaderfile, std::string texturefile) {
		
		constantFile = constantfile;
		shaderFile = shaderfile;
		textureFile = texturefile;

		genConstantData(constantFile);
		genShaderProgram(shaderFile);
		genTextureObject(textureFile);

	}

	void drawStars(std::function<void(std::vector<CSVReader::MotionDataFormatCSV>&)> callback_DrawStars) {

		// read motiondata= callback_DrawStars;
		bool check = iterateMotionFileByChunk(motionFile, Chunk, callback_DrawStars);

	}


	bool isConstantsCreated{ false };

	void genConstantData(std::string filename) {
		// Read constants
		std::vector<std::vector<std::string> > data = CSVReader::readFileAll(filename);
		//ConstantFormat element;
		for (std::vector<std::string> c : data) {

			ConstantData element{
				std::stoi(c[0]),
				c[1],
				c[2],
				std::stof(c[3]),
				c[4],
				{},
				std::stoi(c[6]),
				glm::vec3{std::stof(c[7]), std::stof(c[8]), std::stof(c[9])},
				std::stof(c[10]),
				std::stof(c[11]),
				std::stof(c[12]),
				std::stof(c[13]),
				std::stof(c[14]),
				std::stof(c[15]),
				std::stof(c[16]),
				std::stof(c[17]),
				std::stof(c[18]),
				std::stof(c[19]),
				glm::vec3{std::stof(c[20]), std::stof(c[21]), std::stof(c[22])},
				glm::vec3{std::stof(c[23]), std::stof(c[24]), std::stof(c[25])},
				glm::vec3{0.0f}
			};

			boost::algorithm::split(element.texName, c[5], boost::is_any_of(";"));
			constant.emplace(std::stoi(c[0]), element);

		}

		isConstantsCreated = true;
	}


	void genShaderProgram(std::string filename) {

		// Read shader
		std::vector<std::vector<std::string> > data = CSVReader::readFileAll(filename);
		//ConstantFormat element;
		std::vector<std::string> tempContainer{};
		for (std::vector<std::string> s : data) {
			ShaderProgram element{
				s[0],
				(bool)std::stoi(s[1]),
				get_shader_root(s[2]),
				get_shader_root(s[3]),
				s[4] == "NULL" ? s[4] : get_shader_root(s[4]),
				s[5] == "NULL" ? s[5] : get_shader_root(s[5]),
				s[6] == "NULL" ? s[6] : get_shader_root(s[6]),
				nullptr,
				{},
				{},
				{},
				{},
				{},
				{},
				{},
				{}
			};
			if (!element.toUse) continue;

			element.shaderPtr = new MyShader::Shader{
				(element.vert).c_str(),
				(element.frag).c_str(),
				(element.tesc == "NULL" ? nullptr : (element.tesc).c_str()),
				(element.tese == "NULL" ? nullptr : (element.tese).c_str()),
				(element.geom == "NULL" ? nullptr : (element.geom).c_str())
			};

			std::cout << "\nDebug:: " << "Shader Name = " << element.shaderName << ", Shader ID= " << element.shaderPtr->ID << "\n";

			// unofrm location setting
			std::cout << " (UnifLocName, UNifLocNo)-> ";

			boost::algorithm::split(element.unifLocName, s[7], boost::is_any_of(";"));
			GLuint locno;
			for (std::string locname : element.unifLocName) {
				if (locname == "") {// means Nothing
					element.unifLocName = {}; // to erase ""
					std::cout << "\tNone";
					break;
				}
				locno = glGetUniformLocation(element.shaderPtr->ID, locname.c_str());
				element.unifLocNo.push_back(locno);

				std::cout << "(" << locname << ", " << locno << "); ";
			}
			std::cout << std::endl;

			// Generate unifBlocks
			std::cout << " (UnifBlockName, BindingIndex, BlockIndex)->";

			boost::algorithm::split(element.unifBlockName, s[8], boost::is_any_of(";"));
			tempContainer.clear();
			boost::algorithm::split(tempContainer, s[9], boost::is_any_of(";"));
			std::string blockname;
			std::string bindingindex;
			GLuint blockindex;
			for (auto tup : boost::combine(element.unifBlockName, tempContainer)) {
				boost::tie(blockname, bindingindex) = tup;
				if (blockname == "") {// means Nothing
					element.unifBlockName = {}; // erase ""
					std::cout << "\tNone";
					break;
				}
				element.unifBlockBindingIndex.push_back(std::stoi(bindingindex));
				// block binding
				blockindex = glGetUniformBlockIndex(element.shaderPtr->ID, blockname.c_str());
				glUniformBlockBinding(element.shaderPtr->ID, blockindex, (GLuint)std::stoi(bindingindex));
				GLenum info = glGetError();
				element.unifBlockIndex.push_back(blockindex);
				// set up unifBlocks map, without duplication
				unifBlocks.emplace(blockname, UnifBlocks{ blockname, (GLuint)std::stoi(bindingindex) });

				std::cout << "(" << blockname << ", " << bindingindex << ", " << blockindex << ", Info = " << info << "); ";

			}
			std::cout << std::endl;

			// texture unit number setting
			std::cout << " (TexUnifLocName, TexUnifLocNo)-> ";

			boost::algorithm::split(element.texUnifLocName, s[10], boost::is_any_of(";"));
			tempContainer.clear();
			boost::algorithm::split(tempContainer, s[11], boost::is_any_of(";"));
			std::string texlocname;
			std::string texunitno;
			for (auto tup : boost::combine(element.texUnifLocName, tempContainer)) {
				boost::tie(texlocname, texunitno) = tup;
				if (texlocname == "") {// means Nothing
					element.texUnifLocName = {}; // erase ""
					std::cout << "\tNone";
					break;
				}
				element.texUnitNo.push_back(std::stoi(texunitno));
				glUseProgram(element.shaderPtr->ID);
				glUniform1i(glGetUniformLocation(element.shaderPtr->ID, texlocname.c_str()),
					std::stoi(texunitno));
				glUseProgram(0);

				std::cout << "(" << texlocname << ", " << texunitno << "); ";
			}

			shader.emplace(s[0], element);

			std::cout << std::endl;
		}

		// setup uniform block buffer, after completing shader programs
		std::cout << "\nDebug:: Uniform Block\n";
		std::string blockname;
		for (auto b : unifBlocks) {
			blockname = b.first;// key
			glGenBuffers(1, &unifBlocks.at(blockname).unifBuffObject);
			glBindBuffer(GL_UNIFORM_BUFFER, unifBlocks.at(blockname).unifBuffObject);
			GLenum info = glGetError();
			glBufferData(GL_UNIFORM_BUFFER, unifBlocks.at(blockname).unifBlockSize, NULL, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferRange(GL_UNIFORM_BUFFER, unifBlocks.at(blockname).unifBindingIndex, unifBlocks.at(blockname).unifBuffObject, 0, unifBlocks.at(blockname).unifBlockSize);

			std::cout << " BlockName= " << blockname << ", Buffer Object= " << unifBlocks.at(blockname).unifBuffObject << ", Binding Index= " << unifBlocks.at(blockname).unifBindingIndex << ", Block Size= " << unifBlocks.at(blockname).unifBlockSize << ", Bind Error= " << info << "\n";

		}
		std::cout << std::endl;

		// MVPBlock subdata offset and size
		mvpSubData.emplace(subDataType::model, MVPBlockSubData{ subDataType::model });
		mvpSubData.emplace(subDataType::view, MVPBlockSubData{ subDataType::view });
		mvpSubData.emplace(subDataType::projection, MVPBlockSubData{ subDataType::projection });

	}


	void genTextureObject(std::string filename) {
		// Read texturer
		std::vector<std::vector<std::string> > data = CSVReader::readFileAll(filename);
		//ConstantFormat element;
		bool flip_vertically = false;
		for (std::vector<std::string> t : data) {
			TextureData element{
				t[0],
				(t[1] == "NULL") ? "NULL" : get_globaldata_root(t[1]),
				(GLuint)std::stoi(t[2]),
				t[3],
				(t[1] == "NULL") ? 0 : loadTexture((element.texFile).c_str(), flip_vertically)
			};

			//element.texObject = (t[1] == "NULL") ? NULL : loadTexture(element.texFile.c_str(), flip_vertically);
			texture.emplace(element.texName, element);

		}
	}

	enum class DifferentialAlgorism {
		feynman,
		rungekutta
	};

	class SolarMotionData {
	public:
		int tailLineLength{0};
		const float Gm = 6.67430e-11f; // Gravity constant  6.67 x 10(-11) Nm(2)/kg(2)
		const float Am = 1.495979e+11f; // Astornomical unit in meter
		const float Am2 = pow(Am, 2.0f);
		const int Pau = 200; //one au in pixel
		const int Px2 = (int)pow(Pau, 2.0f);
		const float Gpx0 = (Gm / Am2) * Px2; //Gm now converted to display unit
		//coeffcient to calculate velociy in pixel / day
		const float Ds = 60.0f * 60.0f * 24.0f; //seconds / day
		const float Pday = (1.0f / Am) * Pau * Ds; // pixel / day: velocity(m / s) * Pday :
		const float Gpx = Gpx0 * (Pau * pow(Ds, 2.0f) / Am); //Newton : converted to kg •Å@pixel / day(2)

		std::map<int, ConstantData> Constant;
		std::vector<std::vector<MotionDataFormat>> motionDataContainer;

		SolarMotionData(float starttime, float timestep, int repeats, DifferentialAlgorism algorism, std::map<int, ConstantData> & constant, int containersize=10) :Constant(constant), sysTimer(starttime), timeStep(timestep), Repeats(repeats) {
			//sysTimer = starttime;
			//timeStep = timestep;
			//Repeats = repeats;

			// initial data setting
			//Constant = constant;
			tailLineLength = int(containersize / timeStep);  // containersize in days
			repeat = 0;
			motionData = {};
			motionDataContainerRow.clear();
			for (auto c: Constant){
				motionData.repeat = repeat;
				motionData.systimer = sysTimer;
				motionData.planetId = c.first;
				motionData.position = Pau * (1000.0f / Am) * c.second.initPosition; // in pixel
				motionData.velocity = Pday * c.second.initVelocity; // in pixel
				motionData.momentum = glm::vec3{ 0.0f };  // data to be set
				motionData.orbitalAngularMomentum = glm::vec3{ 0.0f };  // data to be set
				motionData.spinAngularMomentum = glm::vec3{ 0.0f };  // data to be set

				motionDataContainerRow.push_back(motionData); // vector array[100][14], to be coded as stack later
			}


			//acceleration(motionDataContainerRow);
			scalePixelPosition(motionDataContainerRow);

			// Make motionDataContainer 100 as initial state
			motionDataContainer.clear();
			for (int i = 0; i < tailLineLength; i++) {
				motionDataContainer.push_back(motionDataContainerRow);
			}

		}

		void updateMotionData(bool debug) {

			if (debug == true) {
				//Debug Output MotionData
				debugWriteMotionData(get_localdata_root("other/solarsys_debug_motiondata.csv"));
				// Debug End

			}

			motionDataContainerRow.clear();
			motionDataContainerRow = motionDataContainer.back();
			// tick timer and repeat
			for (auto & m:  motionDataContainerRow) {
				m.systimer += timeStep;
				m.repeat += 1;
			}
			rungekutta(motionDataContainerRow);
			scalePixelPosition(motionDataContainerRow);
			motionDataContainer.push_back(motionDataContainerRow);
			motionDataContainer.erase(motionDataContainer.begin());

		}


		void drawStars(std::function<void()> callback_DrawStars) {

			callback_DrawStars();
				
			}


		bool first_write{true};
		void debugWriteMotionData(std::string filename) {
			std::ofstream outfile;
			outfile.open(filename, std::ios::app);
			int size;
			const char* buffer;
			std::string delimiter = ",";
			std::string line;
			if (first_write) {
				std::string headline =
					"repeat" + delimiter +
					"systimer" + delimiter +
					"planetId" + delimiter +
					"planetName" + delimiter +
					"scaledPositionX" + delimiter +
					"scaledPositionY" + delimiter +
					"scaledPositionY" + delimiter +
					"positionX" + delimiter +
					"positionY" + delimiter +
					"positionZ" + delimiter +
					"velocityX" + delimiter +
					"velocityy" + delimiter +
					"velocityZ" + delimiter +
					"accelerationX" + delimiter +
					"accelerationY" + delimiter +
					"accelerationZ" + delimiter +
					"momentumX" + delimiter +
					"momentumY" + delimiter +
					"momentumZ" + delimiter +
					"orbitalAngularMomentumX" + delimiter +
					"orbitalAngularMomentumY" + delimiter +
					"orbitalAngularMomentumZ" + delimiter +
					"spinAngularMomentumX" + delimiter +
					"spinAngularMomentumY" + delimiter +
					"spinAngularMomentumZ" + "\n";
				size = headline.size();
				buffer = new char[size];
				buffer = headline.c_str();
				outfile.write(buffer, size);

				first_write = false;
			}
			
			motionDataContainerRow = motionDataContainer.back();
			for (auto &motion: motionDataContainerRow) {
				line = {};
				line.append(std::to_string(motion.repeat)+delimiter);
				line.append(std::to_string(motion.systimer) + delimiter);
				line.append(std::to_string(motion.planetId) + delimiter);
				line.append(Constant.at(motion.planetId).planetName + delimiter);
				line.append(std::to_string(motion.scaledPosition.x) + delimiter);
				line.append(std::to_string(motion.scaledPosition.y) + delimiter);
				line.append(std::to_string(motion.scaledPosition.z) + delimiter);
				line.append(std::to_string(motion.position.x) + delimiter);
				line.append(std::to_string(motion.position.y) + delimiter);
				line.append(std::to_string(motion.position.z) + delimiter);
				line.append(std::to_string(motion.velocity.x) + delimiter);
				line.append(std::to_string(motion.velocity.y) + delimiter);
				line.append(std::to_string(motion.velocity.z) + delimiter);
				line.append(std::to_string(motion.acceleration.x) + delimiter);
				line.append(std::to_string(motion.acceleration.y) + delimiter);
				line.append(std::to_string(motion.acceleration.z) + delimiter);
				line.append(std::to_string(motion.momentum.x) + delimiter);
				line.append(std::to_string(motion.momentum.y) + delimiter);
				line.append(std::to_string(motion.momentum.z) + delimiter);
				line.append(std::to_string(motion.orbitalAngularMomentum.x) + delimiter);
				line.append(std::to_string(motion.orbitalAngularMomentum.y) + delimiter);
				line.append(std::to_string(motion.orbitalAngularMomentum.z) + delimiter);
				line.append(std::to_string(motion.spinAngularMomentum.x) + delimiter);
				line.append(std::to_string(motion.spinAngularMomentum.y) + delimiter);
				line.append(std::to_string(motion.spinAngularMomentum.z)+"\n");

				size = line.size();
				buffer = new char[size];
				buffer = line.c_str();

				outfile.write(buffer, size);
				
			}
			

		}



	private:

		float sysTimer;
		float timeStep;
		int repeat;
		int Repeats;
		MotionDataFormat motionData{};
		std::vector<MotionDataFormat> motionDataContainerRow;

		void scalePixelPosition(std::vector<MotionDataFormat>& motion) {
			/*
				Convert Au(astronomical unit to PX(pixel on window) in motiondata
				Must be used only after the completion of calculation velocity and accellation
			*/

			//for (int i = 0; i < Constant.size(); i++) { 
			//	int planetId = motion[i].planetId;// motion[i].planetId;
			//	int parentId = Constant.at(planetId).satteliteOfId;
			//	motion[i].scaledPosition = (motion[i].position - motion[parentId].position) * Constant.at(planetId).pixelScaler + motion[parentId].position;
			//	//motion[i].scaledPosition = (motion[i].position - motion[parentId].position) * Constant.at(planetId).pixelScaler + motion[parentId].position;
			//}

			for (auto &s: motion) {
				int planetId = s.planetId;// motion[i].planetId;
				int parentId = Constant.at(planetId).satteliteOfId -1; // need to subtrac 1 for vector index; Constant is map with key as planetId;
				s.scaledPosition = (s.position - motion[parentId].position) * Constant.at(planetId).pixelScaler + motion[parentId].position;
			}
				
			/*
			C = df.loc[:, ('positionX', 'positionY', 'positionZ')].to_numpy()
					P = pd.concat([df[df.planetId == i].loc[:, ('positionX', 'positionY', 'positionZ')] for  i in df.satelliteOfId], ignore_index = True).to_numpy()
					PX = df.loc[:, ('scalerX', 'scalerY', 'scalerZ')].to_numpy()
					df.loc[:, ('pixelX', 'pixelY', 'pixelZ')] = (C - P) * PX + P

					return df
			*/

		}


		void acceleration(std::vector<MotionDataFormat> &motion) {

			/*
			c : planets.constants  # no use
				d : motion data container
				pos : stars position vectors as argument
				pos = np.array(list(zip(d._xx, d._yy, d_zz)))
				see Feynman Physics
				calculate acceleration at time 0 and after
				a(t) = -Gpx * m * r / |r | (3)      a and r are vectors
				r : vector(in pixel)
				m : counter planet
				Gpx : gravity constant in pixel
				"""

			*/
				
				//# calcualte position vectors for each star to others, with itself centered
				//# deltaPos.ndim is 3 - dimention
			glm::vec3 A{ 0.0f, 0.0f, 0.0f };
			glm::vec3 distanceR{ 0.0f, 0.0f, 0.0f };
			float distance{ 0.0f };
			float mass{ 0.0f };
			//int planetId{ 0 };
			for (auto & s : motion) {
				A = glm::vec3{ 0.0f, 0.0f, 0.0f };
				//distanceR = -s.position;
				for (auto& other : motion) {
					if (s.planetId != other.planetId) {
					distanceR = other.position - s.position;;
					distance = glm::length(distanceR);
					//planetId = other.planetId;
					mass = Constant.at(other.planetId).mass;
					A += Gpx * mass * distanceR / (float)pow(distance, 3);
					}
				}

				s.acceleration = A;
			}
			/*
			R = np.array([posv3 - posv3[i - 1, :] for i in self.planetId])

				#pair - wise distance
				distR = squareform(pdist(posv3))
				distR = distR.reshape((distR.shape + (1, )))  # conform to the same dimention as deltaPos for later calc.
				# counter mass
				mass = self.constant.mass.to_numpy() # although including self, mass* R makes itself none
				mass = mass.reshape((mass.shape + (1, )))  # conform to 2 dimensions

				with np.errstate(divide = 'ignore', invalid = 'ignore') :
				accel = (np.nan_to_num(self.Gpx * mass * R / np.power(distR, 3))).sum(axis = 1)
				#accel = accel.sum(axis = 1)   # we get acceleration by star

				return accel   # numpy ndarray
			*/
				
		}

		// Motion Algorism
		void rungekutta(std::vector<MotionDataFormat>& motion) {
			//value of integral f at a given var
			/*
			4th order Runge - Kutta(RK4) method
			https ://lpsa.swarthmore.edu/NumInt/NumIntFourth.html

		dy(t) / dt = y'(t) = f(y(t), t),    with y(t0) = y0
			h : time step

			k1 = f(y * (t0), t0) = y'*(t0)              p*(t0) is an approximation to p(t0)
			k2 = f(y * (t0)+k1 * h / 2, t0 + h / 2) = y'*(t0) + k1 * h/2
			k3 = f(y * (t0)+k2 * h / 2, t0 + h / 2) = y'*(t0) + k2 * h/2
			k4 = f(y * (t0)+k3 * h, t0 + h) = y'*(t0) + k3 * h

			y * (t0 + h) = y * (t0)+(k1 * 1 / 6 + k2 * 1 / 3 + k3 * 1 / 3 + k4 * 1 / 6) * h

			Example given as follows :
		dv / dt = a(t), dp / dt = v(t), a = GM * mass / power(r, 2)
			initial values : t = t0, p(t0) = p0, v(t0) = v0, a(t0) = a0
			h : time step

			step1 : at t0
			a0 = a(p0)
			v0 = v(t0)
			p0 = p(t0)

			step2 : at t0 + h / 2
			v1 = v0 + a0 * h / 2
			p1 = p0 + v1 * h / 2
			a1 = a(p1)

			step3 : at t0 + h / 2
			v2 = v0 + a1 * h / 2
			p2 = p0 + v2 * h / 2
			a2 = a(p2)

			step4 : at t0 + h
			v3 = v0 + a2 * h
			p3 = p0 + v3 * h
			a3 = a(p3)

			finally, at t0 + h, we calculate :
		p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
			v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
			a = a(p)

			we get(p, v, a)

			Repeating the above steps, we get the trajectory of a motion
			*/

			float h = timeStep;
			std::vector<MotionDataFormat>M0 = motion;
			acceleration(M0);

			std::vector<MotionDataFormat>M1= M0;
			for (int i = 0; i < motion.size(); i++) {
				M1[i].position = M0[i].position + M0[i].velocity * h / 2.0f;
				M1[i].velocity = M0[i].velocity + M0[i].acceleration * h / 2.0f;
			}
			acceleration(M1);
			
			std::vector<MotionDataFormat>M2 = M1;
			for (int i = 0; i < motion.size(); i++) {
				M2[i].position = M0[i].position + M1[i].velocity * h / 2.0f;
				M2[i].velocity = M0[i].velocity + M1[i].acceleration * h / 2.0f;
			}
			acceleration(M2);

			std::vector<MotionDataFormat>M3 =  M2;
			for (int i = 0; i < motion.size(); i++) {
				M3[i].position = M0[i].position + M2[i].velocity * h;
				M3[i].velocity = M0[i].velocity + M2[i].acceleration * h;
			}
			acceleration(M3);


			for (int i = 0; i < motion.size(); i++) {
				motion[i].position = M0[i].position + (M0[i].velocity / 6.0f + M1[i].velocity / 3.0f + M2[i].velocity / 3.0f + M3[i].velocity / 6.0f) * h;
				motion[i].velocity = M0[i].velocity + (M0[i].acceleration / 6.0f + M1[i].acceleration / 3.0f + M2[i].acceleration / 3.0f + M3[i].acceleration / 6.0f) * h;
			}
			acceleration(motion);

			////debug
			//for (int i = 0; i < motion.size(); i++) {
			//	if (M0[i].position == motion[i].position) {
			//		std::cout << i << " Position  ->old motion==new motion: True" << std::endl;
			//	}
			//}
			//for (int i = 0; i < motion.size(); i++) {
			//	if (M0[i].velocity == motion[i].velocity) {
			//		std::cout << i << " Velocity  ->old motion==new motion: True" << std::endl;
			//	}
			//}
			//for (int i = 0; i < motion.size(); i++) {
			//	if (M0[i].acceleration == motion[i].acceleration) {
			//		std::cout << i << " Acceleration  ->old motion==new motion: True" << std::endl;
			//	}
			//}
			//// debug end
			/*
				// p0, vo are arguments
				a0 = self.acceleration(p0)
				p1 = p0 + v0 * h / 2
				v1 = v0 + a0 * h / 2

				a1 = self.acceleration(p1)

				v2 = v0 + a1 * h / 2
				p2 = p0 + v1 * h / 2
				a2 = self.acceleration(p2)

				v3 = v0 + a2 * h
				p3 = p0 + v2 * h
				a3 = self.acceleration(p3)

				v = v0 + (a0 / 6 + a1 / 3 + a2 / 3 + a3 / 6) * h
				p = p0 + (v0 / 6 + v1 / 3 + v2 / 3 + v3 / 6) * h
				a = self.acceleration(p)

				return (p, v, a)  //at next time step
			*/


		}


	};
	
	
}

// SOLARTOOL2_H
#endif